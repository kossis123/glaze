import rclpy
from rclpy.node import Node

from sensor_msgs.msg import PointCloud2
from sensor_msgs_py import point_cloud2

class PointCloudNode(Node):

    def __init__(self):
        super().__init__("pointcloud_node")

        self.subscription = self.create_subscription(
            PointCloud2,
            "/camera/points",     
            self.callback,
            10
        )

    def callback(self, msg):

        for point in point_cloud2.read_points(
                msg,
                field_names=("x", "y", "z"),
                skip_nans=True):

            x, y, z = point
            print(x, y, z)
            break      

def main():
    rclpy.init()
    node = PointCloudNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()