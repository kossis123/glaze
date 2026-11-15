import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
from canable_py.CANable import CANableUSBCAN as wave
from canable_py.Motor import MotorMIT
import time


class node(Node):
    def __init__(self):
        super().__init__("Koshish")
        self.sub_=self.create_subscription(
            Float64MultiArray,
            "/velocity",
            self.callback,
            10
        )
        self.UPPER_LIMIT=30
        self.LOWER_LIMIT=-30

        self.declare_parameter("FL",13)
        self.declare_parameter("FR",10)
        self.declare_parameter("RL",12)
        self.declare_parameter("RR",11)
        self.declare_parameter("channel","can0")

        FL=self.get_parameter("FL").value
        FR=self.get_parameter("FR").value
        RL=self.get_parameter("RL").value
        RR=self.get_parameter("RR").value

        can=wave()
        can.start_rx()
        self.FL_MOTOR=MotorMIT(can,motor_id=FL,motor_type="cg")
        self.FR_MOTOR=MotorMIT(can,motor_id=FR,motor_type="cg")
        self.RL_MOTOR=MotorMIT(can,motor_id=RL,motor_type="cg")
        self.RR_MOTOR=MotorMIT(can,motor_id=RR,motor_type="cg")

        motors=[self.FL_MOTOR,
                self.FR_MOTOR,
                self.RL_MOTOR,
                self.RR_MOTOR]

        for motor in motors:
            motor.exit_mit()
            time.sleep(0.1)
            motor.zero_position()
            time.sleep(0.1)
            motor.enter_mit()
            time.sleep(0.1)

    def callback(self,msg):

        self.FL_MOTOR.send(p=0.0,v=msg.data[0],kp=0.0,kd=1.0,t=0.0)
        self.FR_MOTOR.send(p=0.0,v=msg.data[1],kp=0.0,kd=1.0,t=0.0)
        self.RL_MOTOR.send(p=0.0,v=msg.data[2],kp=0.0,kd=1.0,t=0.0)
        self.RR_MOTOR.send(p=0.0,v=msg.data[3],kp=0.0,kd=1.0,t=0.0)
        self.get_logger().info(f"FL: {self.FL_MOTOR.read()}")
        self.get_logger().info(f"FR: {self.FR_MOTOR.read()}")
        self.get_logger().info(f"RL: {self.RL_MOTOR.read()}")
        self.get_logger().info(f"RR: {self.RR_MOTOR.read()}")
        
def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()

if __name__=="__main__":
    main()

