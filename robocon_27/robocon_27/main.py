
import time
from CANable import CANableUSBCAN as WaveshareUSBCAN
from Motor import MotorMIT

can = WaveshareUSBCAN(channel="can1")
can.start_rx()
motor=MotorMIT(can,motor_id=0x7F,motor_type="cg")
motor.exit_mit()
time.sleep(0.1)
motor.zero_position()
time.sleep(0.1)
motor.enter_mit()
time.sleep(0.1)
while True:
    motor.send(p=0.0, v=0.0,kp=2.0,kd=1.0,t=0.0)
    time.sleep(0.03)
    print(motor.read())




   

