#include <cmath>
#include <memory>
#include<algorithm>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class CmdVelToWheels : public rclcpp::Node
{
public:
    CmdVelToWheels()
        : Node("cmd_vel_to_wheels")
    {
        wheel_radius_ = 0.08;
        wheel_separation_ = 0.36;

        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(
                &CmdVelToWheels::cmdVelCallback,
                this,
                std::placeholders::_1));

        wheel_pub_ =
            this->create_publisher<std_msgs::msg::Float64MultiArray>(
                "/wheel/commands",
                10);

        RCLCPP_INFO(this->get_logger(),
                    "Twist -> Wheel converter started");
    }

private:

    void cmdVelCallback(
        const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        double v = msg->linear.x;
        double omega = msg->angular.z;

        double left_velocity =
            (v - omega * wheel_separation_ / 2.0)
            / wheel_radius_;

        double right_velocity =
            (v + omega * wheel_separation_ / 2.0)
            / wheel_radius_;

        // Safety limit from your ros2_control interfaces
        left_velocity =
            std::clamp(left_velocity, -20.0, 20.0);

        right_velocity =
            std::clamp(right_velocity, -20.0, 20.0);

        std_msgs::msg::Float64MultiArray command;

        command.data.push_back(left_velocity);
        command.data.push_back(right_velocity);

        wheel_pub_->publish(command);
    }

    double wheel_radius_;
    double wheel_separation_;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr
        cmd_vel_sub_;

    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr
        wheel_pub_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<CmdVelToWheels>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}