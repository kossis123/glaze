#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

class VelocityPublisher : public rclcpp::Node
{
private:

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;

    rclcpp::TimerBase::SharedPtr timer_;

    float vx = 2.0;
    float vy = 0.0;
    float omega = 0.0;

public:

    VelocityPublisher() : Node("velocity_publisher")
    {
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10
        );

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(20),
            std::bind(&VelocityPublisher::publish_velocity, this)
        );

        RCLCPP_INFO(this->get_logger(), "Velocity publisher started");
    }

    void publish_velocity()
    {
        geometry_msgs::msg::Twist msg;

        msg.linear.x = vx;
        msg.linear.y = vy;
        msg.angular.z = omega;

        pub_->publish(msg);
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<VelocityPublisher>());

    rclcpp::shutdown();

    return 0;
}