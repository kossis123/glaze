import json
import logging
import os
import subprocess
import threading
import time

import can

log = logging.getLogger(__name__)


class CANableUSBCAN:
    """
    Drop-in replacement for WaveshareUSBCAN, for the MKS CANable V2.0
    running candleLight firmware (shows up as a SocketCAN interface, e.g. can0).

    Same public API as the Waveshare driver:
        send_can(can_id, data, extended=False, remote=False)
        start_rx() / stop_rx()
        get_latest(can_id) / get_latest_snapshot()
        close(), and use as a context manager (starts RX on enter)

    Bitrate is set from code. The bitrate of a SocketCAN interface can only be
    changed by root (`ip link set ... type can bitrate N`), so:
      - if the interface is already UP at the requested bitrate, nothing is run
        and no privileges are needed
      - otherwise the driver runs the `ip` commands itself, using `sudo -n`
        (no password prompt) unless you are already root
    See the sudoers note in the README/reply for passwordless setup.
    """

    def __init__(self, channel="can0", can_baud=1_000_000, configure=True,
                 txqueuelen=1000, strict=False):
        """
        channel    : SocketCAN interface name, e.g. "can0"
        can_baud   : CAN bitrate in bit/s (CyberGear needs 1_000_000)
        configure  : if True, bring the interface up at can_baud when needed
        txqueuelen : TX queue length set when configuring (default kernel value is 10)
        strict     : if True, send failures raise; if False they are counted in
                     self.tx_errors and logged (keeps a control loop alive when
                     the bus has no ACKing node or the TX queue is full)
        """
        self.channel   = channel
        self.can_baud  = can_baud
        self.strict    = strict
        self.tx_errors = 0

        if configure:
            self._ensure_interface(channel, can_baud, txqueuelen)

        self.bus = can.Bus(interface="socketcan", channel=channel)

        self._rx_thread = None
        self._running   = False
        self._lock      = threading.Lock()
        self._latest    = {}


    @staticmethod
    def _link_state(channel):
        """Return (is_up, bitrate or None) for the interface."""
        r = subprocess.run(
            ["ip", "-details", "-json", "link", "show", channel],
            capture_output=True, text=True,
        )
        if r.returncode != 0:
            raise RuntimeError(
                f"CAN interface '{channel}' not found ({r.stderr.strip()}). "
                "Is the adapter plugged in and the gs_usb module loaded?"
            )
        try:
            info    = json.loads(r.stdout)[0]
            is_up   = "UP" in info.get("flags", [])
            bitrate = (info.get("linkinfo", {})
                           .get("info_data", {})
                           .get("bittiming", {})
                           .get("bitrate"))
            return is_up, bitrate
        except (ValueError, IndexError, KeyError):
            return False, None

    @staticmethod
    def _run_ip(args):
        cmd = ["ip"] + args
        if os.geteuid() != 0:
            cmd = ["sudo", "-n"] + cmd
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            raise RuntimeError(
                f"'{' '.join(cmd)}' failed: {r.stderr.strip() or r.stdout.strip()}\n"
                "Either run once by hand, e.g.\n"
                "  sudo ip link set can0 up type can bitrate 1000000\n"
                "or add a sudoers rule so the script can do it without a password."
            )

    def _ensure_interface(self, channel, bitrate, txqueuelen):
        is_up, current = self._link_state(channel)
        if is_up and current == bitrate:
            log.info("%s already up at %d bit/s", channel, bitrate)
            return
        log.info("configuring %s at %d bit/s", channel, bitrate)
        self._run_ip(["link", "set", channel, "down"])
        self._run_ip(["link", "set", channel, "up", "type", "can",
                      "bitrate", str(bitrate)])
        if txqueuelen:
            self._run_ip(["link", "set", channel, "txqueuelen", str(txqueuelen)])


    def send_can(self, can_id, data, extended=False, remote=False):
        """
        Transmit one CAN frame.

        can_id   : int — 11-bit (standard) or 29-bit (extended)
        data     : list/bytes, 0-8 bytes
        extended : True for 29-bit extended frame (required for CyberGear)
        remote   : True for remote-request frame (no payload)
        """
        data = bytes(data)
        if len(data) > 8:
            raise ValueError("CAN payload must be 0-8 bytes")
        if not extended and can_id > 0x7FF:
            raise ValueError("Standard frame ID must be 11-bit (<=0x7FF)")
        if extended and can_id > 0x1FFFFFFF:
            raise ValueError("Extended frame ID must be 29-bit (<=0x1FFFFFFF)")

        msg = can.Message(
            arbitration_id=can_id,
            data=b"" if remote else data,
            dlc=len(data),
            is_extended_id=extended,
            is_remote_frame=remote,
        )
        try:
            self.bus.send(msg, timeout=0.05)
        except can.CanError as e:
            self.tx_errors += 1
            if self.strict:
                raise
            if self.tx_errors == 1 or self.tx_errors % 100 == 0:
                log.warning("CAN send failed (%d so far): %s", self.tx_errors, e)


    def _rx_loop(self):
        """
        Background RX thread. Stores the latest frame per key, using the same
        keying and fields as the Waveshare driver so Motor.py keeps working.
        """
        while self._running:
            try:
                msg = self.bus.recv(timeout=0.1)
            except can.CanError as e:
                log.warning("CAN recv error: %s", e)
                time.sleep(0.05)
                continue
            if msg is None or msg.is_error_frame:
                continue

            can_id    = msg.arbitration_id
            can_data  = list(msg.data)
            dlc       = msg.dlc
            is_ext    = msg.is_extended_id
            is_remote = msg.is_remote_frame

            if is_ext:
                motor_id  = (can_id >> 8) & 0xFF
                comm_type = (can_id >> 24) & 0x1F
                with self._lock:
                    self._latest[motor_id] = {
                        "data":      can_data,
                        "extended":  True,
                        "remote":    is_remote,
                        "dlc":       dlc,
                        "comm_type": comm_type,
                        "raw_id":    can_id,
                    }
            else:
                with self._lock:
                    self._latest[can_id] = {
                        "data":     can_data,
                        "extended": False,
                        "remote":   is_remote,
                        "dlc":      dlc,
                    }

    def start_rx(self):
        """Start background RX thread. Safe to call multiple times."""
        if self._rx_thread:
            return
        self._running   = True
        self._rx_thread = threading.Thread(target=self._rx_loop, daemon=True)
        self._rx_thread.start()

    def stop_rx(self):
        """Stop background RX thread."""
        self._running = False
        if self._rx_thread:
            self._rx_thread.join(timeout=0.5)
            self._rx_thread = None

    def get_latest(self, can_id):
        """Most recent frame for the given key (see class notes), or None."""
        with self._lock:
            return self._latest.get(can_id)

    def get_latest_snapshot(self):
        """Shallow copy of the full _latest dict (thread-safe)."""
        with self._lock:
            return dict(self._latest)

    def close(self):
        self.stop_rx()
        self.bus.shutdown()

    def __enter__(self):
        self.start_rx()
        return self

    def __exit__(self, *_):
        self.close()
