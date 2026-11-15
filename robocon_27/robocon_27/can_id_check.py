
import time
from robocon_27.CANable import CANableUSBCAN as WaveshareUSBCAN
from robocon_27.Motor import MotorMIT

def callback(i):
    can = WaveshareUSBCAN( )
    can.start_rx()
    motor=MotorMIT(can,motor_id=i,motor_type="cg")
    motor.exit_mit()
    time.sleep(0.1)
    motor.zero_position()
    time.sleep(0.1)
    motor.enter_mit()
    time.sleep(0.1)

    while True:
        motor.send(p=0.0, v=12.0,kp=0.0,kd=1.0,t=0.0)
        print(motor.read(),str(i))
        time.sleep(0.05)
        if motor.read()!=None:
            return 1
        break

for i in range(100,128):
    a=callback(i)
    if (a==1):
        break


    

