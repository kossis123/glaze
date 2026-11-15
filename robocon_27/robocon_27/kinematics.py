from sensor_msgs.msg import Joy
import rclpy
from rclpy.node import Node
import numpy as np
from std_msgs.msg import Float64MultiArray

class node(Node):
    def __init__(self):
        super().__init__("Koshish")
        self.LOWER_LIMIT=-10.0
        self.UPPER_LIMIT=10.0
        self.sub_=self.create_subscription(Joy,
                                           "/joy",
                                           self.callback,
                                           10)
        self.pub_=self.create_publisher(Float64MultiArray,
                                        "/velocity",
                                        10
        )
        self.declare_parameter("lx",0.25)
        self.declare_parameter("ly",0.25)
        self.declare_parameter("r",0.075)

        self.lx=self.get_parameter("lx").value
        self.ly=self.get_parameter("ly").value
        self.r=self.get_parameter("r").value

    def callback(self,msg):

        array=Float64MultiArray()
        L=self.lx + self.ly
        VX=msg.axes[1]
        VY=msg.axes[3]
        W=msg.axes[0]
        VX=np.interp(VX,[-1,1],[-2,2])
        VY=np.interp(VY,[-1,1],[-2,2])
        W=np.interp(W,[-1,1],[-10,10])

        W_FR = (VX - VY - L * W) / self.r
        W_FL = (VX + VY + L * W) / self.r
        W_RR = (VX + VY - L * W) / self.r
        W_RL = (VX - VY + L * W) / self.r

        # W_FR=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,W_FR))
        # W_FL=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,W_FL))
        # W_RR=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,W_RR))
        # W_RL=max(self.LOWER_LIMIT,min(self.UPPER_LIMIT,W_RL))
        array.data=[float(W_FL),float(W_FR),float(W_RL),float(W_RR)]
        self.get_logger().info(f"w1:{(W_FL)} w2:{(W_FR)} w3:{(W_RL)} w4:{(W_RR)}")
        self.pub_.publish(array)


def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()
