import rclpy
from rclpy.node import Node
from canable_py.CANable import CANableUSBCAN as wave
from canable_py.Motor import MotorMIT 
import time
from sensor_msgs.msg import Joy

class node(Node):
    def __init__(self):

        super().__init__("Koshish")
        self.sub_=self.create_subscription(
            Joy,
            "/joy",
            self.callback,
            10
        )
        self.UPPER_LIMIT=30
        self.LOWER_LIMIT=-30
        
        self.declare_parameter("channel","can0")
        self.declare_parameter("motor_id",1)
        channel=self.get_parameter("channel").value
        motor_id=self.get_parameter("motor_id").value
        
        self.can=wave(channel=channel)
        self.can.start_rx()
        self.motor=MotorMIT(self.can,motor_id=motor_id,motor_type="cg")
        self.motor.exit_mit()
        time.sleep(0.1)
        self.motor.zero_position()
        time.sleep(0.1)
        self.motor.enter_mit()
        time.sleep(0.1)

    def callback(self,msg):
        self.X=msg.axes[1]
        self.y=msg.axes[3]
        KP=0.0
        section=0.095

    
        KP= self.X * self.UPPER_LIMIT
        KP=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,KP))
        self.motor.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
        self.get_logger().info(str(self.X)+str(self.y))

def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()