#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>

class node : public rclcpp::Node
{
public:

    node() : Node("koshish")
    {
        s_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(&node::callback, this, std::placeholders::_1)
        );

        p_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/wheel/commands",
            10
        );
    }

private:

    void callback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        double velocity = msg->linear.x;
        double angular = msg->angular.z;

        double left_wheel =
            (velocity - angular * wheel_separation_ / 2.0) / radius_;

        double right_wheel =
            (velocity + angular * wheel_separation_ / 2.0) / radius_;

        std_msgs::msg::Float64MultiArray msg2;

        msg2.data = {left_wheel, right_wheel};

        p_->publish(msg2);
    }

    double wheel_separation_ = 0.36;
    double radius_ = 0.08;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr s_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr p_;
};


int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<node>());

    rclcpp::shutdown();

    return 0;
}