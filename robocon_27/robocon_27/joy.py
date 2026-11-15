import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
import time
from robocon_27.CANable import CANableUSBCAN as wave
from robocon_27.Motor import MotorMIT 

class node(Node):
    def __init__(self):
        super().__init__("Koshish")

        self.UPPER_LIMIT=15
        self.LOWER_LIMIT=-15

        self.sub_=self.create_subscription(Joy,
                                           "/joy",
                                           self.callback,
                                           10)

        self.can=wave(channel="can0")
        self.can.start_rx()
        
        self.motor1=MotorMIT(self.can,motor_id=1,motor_type="cg") 
        self.motor2=MotorMIT(self.can,motor_id=18,motor_type="cg") 
        # self.motor3=MotorMIT(self.can,motor_id=3,motor_type="cg")
        # self.motor4=MotorMIT(self.can,motor_id=4,motor_type="cg")  

        motors=[self.motor1,
                self.motor2,
                # self.motor3,
                # self.motor4
        ]

        for motor in motors:
            motor.exit_mit()
            time.sleep(0.05)
            motor.zero_position()
            time.sleep(0.05)
            motor.enter_mit()
            time.sleep(0.05)


    def callback(self,msg):
        self.X=msg.axes[1]
        self.Y=msg.axes[3]
        KP=0.0
        KD=0.0
        section=0.095


        KP= self.X * self.UPPER_LIMIT
        KP=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,KP))
        self.get_logger().info(str(KP))
        self.motor1.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
        self.motor2.send(p=0.0 ,v=KP ,kp=0.0 ,kd=3.0 ,t=0.0)
            # self.motor3.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
            # self.motor4.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
        time.sleep(0.03)

        if(self.Y>0 ):
            KD= -(self.X * self.UPPER_LIMIT)
            KD=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,KD))
            self.get_logger().info(str(KD))
            self.motor1.send(p=0.0 ,v=KD ,kp=0.0 ,kd=1.0 ,t=0.0)
            self.motor2.send(p=0.0 ,v=0 ,kp=0.0 ,kd=1.0 ,t=0.0)
            # self.motor3.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
            # self.motor4.send(p=0.0 ,v=KP ,kp=0.0 ,kd=1.0 ,t=0.0)
            time.sleep(0.03)
        if(self.Y<0):
            KD= self.X * self.UPPER_LIMIT
            KD= max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,KD))
            self.get_logger().info(str(KD))
            self.motor2.send(p=0.0 ,v=0 ,kp=0.0 ,kd=1.0 ,t=0.0)
            self.motor2.send(p=0.0 ,v=KD ,kp=0.0 ,kd=1.0 ,t=0.0)
            time.sleep(0.03)

        
def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()
