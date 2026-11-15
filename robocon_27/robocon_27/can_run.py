import rclpy
from rclpy.node import Node
import time
from robocon_27.CANable import CANableUSBCAN as WaveshareUSBCAN
from robocon_27.Motor import MotorMIT
class node(Node):

    def __init__(self):
        super().__init__("koshish")
        self.can = WaveshareUSBCAN(channel="can0" )
        self.can.start_rx()
        self.motor=MotorMIT(self.can,motor_id=1,motor_type="cg")
        self.motor.exit_mit()
        time.sleep(0.1)
        self.motor.zero_position()
        time.sleep(0.1)
        self.motor.enter_mit()
        time.sleep(0.1)
        self.timer=self.create_timer(0.1,self.callback)
    
    def callback(self):
        self.motor.send(p=3.0, v=0,kp=1.0,kd=2.0,t=0.0)
        time.sleep(0.03)
        print(self.motor.read())



def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()