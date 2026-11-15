import math
import time

from CANable import CANableUSBCAN as WaveshareUSBCAN
from Motor import MotorMIT


class OmniKinematics:

    def __init__(self, wheel_radius, L):
        self.r = wheel_radius
        self.L = L

    def inverse_kinematics(self, vx, vy, w):

        FL = (vx + vy - self.L * w) / (math.sqrt(2) * self.r)
        FR = (vx - vy + self.L * w) / (math.sqrt(2) * self.r)
        RL = (vx - vy - self.L * w) / (math.sqrt(2) * self.r)
        RR = (vx + vy + self.L * w) / (math.sqrt(2) * self.r)

        return FL, FR, RL, RR


class OmniRobot:

    def __init__(self):

        # Initialize CAN
        self.can = WaveshareUSBCAN()
        self.can.start_rx()

        # Initialize motors
        self.FL = MotorMIT(self.can, motor_id=1, motor_type="cg")
        self.FR = MotorMIT(self.can, motor_id=2, motor_type="cg")
        self.RL = MotorMIT(self.can, motor_id=3, motor_type="cg")
        self.RR = MotorMIT(self.can, motor_id=4, motor_type="cg")

        self.motors = [self.FL, self.FR, self.RL, self.RR]

        # Initialize kinematics
        self.kinematics = OmniKinematics(
            wheel_radius=0.075,
            L=0.25
        )

    def initialize_motors(self):

        for motor in self.motors:
            motor.exit_mit()

        time.sleep(0.1)

        for motor in self.motors:
            motor.zero_position()

        time.sleep(0.1)

        for motor in self.motors:
            motor.enter_mit()

        time.sleep(0.1)

        print("All motors initialized!")

    def move(self, vx, vy, w):

        # Calculate wheel velocities
        FL, FR, RL, RR = self.kinematics.inverse_kinematics(
            vx, vy, w
        )

        # Send commands to motors
        self.FL.send(p=0.0, v=FL, kp=0.0, kd=1.0, t=0.0)
        self.FR.send(p=0.0, v=FR, kp=0.0, kd=1.0, t=0.0)
        self.RL.send(p=0.0, v=RL, kp=0.0, kd=1.0, t=0.0)
        self.RR.send(p=0.0, v=RR, kp=0.0, kd=1.0, t=0.0)

    def stop(self):

        self.move(0.0, 0.0, 0.0)

    def shutdown(self):

        self.stop()
        time.sleep(0.1)

        for motor in self.motors:
            motor.exit_mit()

        print("Motors disabled!")


# =====================================
# MAIN
# =====================================

if __name__ == "__main__":

    robot = OmniRobot()

    try:

        robot.initialize_motors()

        while True:

            # Desired robot velocity
            vx = 1.0
            vy = 0.0
            w = 0.0

            robot.move(vx, vy, w)

            time.sleep(0.02)  # 50 Hz

    except KeyboardInterrupt:

        print("\nStopping robot...")

    finally:

        robot.shutdown()