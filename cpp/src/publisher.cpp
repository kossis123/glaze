#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <cmath>
using namespace std::chrono_literals;
class node: public rclcpp::Node
{
    public:
    node():Node("koshsihs")
    {
    s_=this->create_subscription<nav_msgs::msg::Odometry>("/odom",
    10,
    std::bind(&node::callback,this,std::placeholders::_1)
    );
    p_=this->create_publisher<std_msgs::msg::Float64MultiArray>("/wheel/commands",
        10);
    }
    private:
    void callback(const nav_msgs::msg::Odometry::SharedPtr position )
    {

        auto x=position->pose.pose.position.x;
        auto y=position->pose.pose.position.y;
        auto qx=position->pose.pose.orientation.x;
        auto qy=position->pose.pose.orientation.y;
        auto qz=position->pose.pose.orientation.z;
        auto qw=position->pose.pose.orientation.w;


        double dx=_x - x;
        double dy=_y - y;
        double desired_angle=atan2(dy,dx);
        double cos_yaw=1-2*(pow(qy,2)+pow(qz,2));
        double sin_yaw=2*(qw*qz + qx*qy);
        double yaw=atan2(sin_yaw,cos_yaw);
        double error=desired_angle-yaw;
        double dist=sqrt(pow(dx,2)+pow(dy,2));
   
        RCLCPP_INFO(this->get_logger(),"%lf  %lf",yaw,error);
        std_msgs::msg::Float64MultiArray cmd;

        if (std::abs(error) > 0.01)
{
    cmd.data = {0, 3*error, 0, 3*error};
}
else if (dist > 0.1)
{
    cmd.data = {5.0, 5.0, 5.0, 5.0};
}
else
{
    cmd.data = {0.0, 0.0, 0.0, 0.0};
}

        p_->publish(cmd);


    }
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr s_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr p_;
    double _x=10;
    double _y=10;

};
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
    return 0; 

}