#include <chrono>
#include <iostream>
#include <memory>
#include <termios.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class TeleopNode : public rclcpp::Node
{
public:

    TeleopNode()
        : Node("wheel_teleop")
    {
        publisher_ =
            this->create_publisher<std_msgs::msg::Float64MultiArray>(
                "/wheel/commands",
                10
            );

        timer_ = this->create_wall_timer(
            50ms,
            std::bind(&TeleopNode::readKeyboard, this)
        );

        RCLCPP_INFO(
            this->get_logger(),
            "Wheel teleop started"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "W = forward"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "S = backward"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "A = rotate left"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "D = rotate right"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "X = stop"
        );

        RCLCPP_INFO(
            this->get_logger(),
            "Q = quit"
        );
    }


private:

    void readKeyboard()
    {
        char key = getKey();

        double left_velocity = 0.0;
        double right_velocity = 0.0;

        // =========================================
        // FORWARD
        // =========================================

        if (key == 'w' || key == 'W')
        {
            left_velocity = speed_;
            right_velocity = speed_;
        }

        // =========================================
        // BACKWARD
        // =========================================

        else if (key == 's' || key == 'S')
        {
            left_velocity = -speed_;
            right_velocity = -speed_;
        }

        // =========================================
        // ROTATE LEFT
        // =========================================

        else if (key == 'a' || key == 'A')
        {
            left_velocity = -turn_speed_;
            right_velocity = turn_speed_;
        }

        // =========================================
        // ROTATE RIGHT
        // =========================================

        else if (key == 'd' || key == 'D')
        {
            left_velocity = turn_speed_;
            right_velocity = -turn_speed_;
        }

        // =========================================
        // STOP
        // =========================================

        else if (key == 'x' || key == 'X')
        {
            left_velocity = 0.0;
            right_velocity = 0.0;
        }

        // =========================================
        // QUIT
        // =========================================

        else if (key == 'q' || key == 'Q')
        {
            publishVelocity(0.0, 0.0);

            rclcpp::shutdown();

            return;
        }

        else
        {
            return;
        }

        publishVelocity(
            left_velocity,
            right_velocity
        );
    }


    void publishVelocity(
        double left,
        double right)
    {
        std_msgs::msg::Float64MultiArray msg;

        msg.data.push_back(left);
        msg.data.push_back(right);

        publisher_->publish(msg);
    }


    char getKey()
    {
        struct termios old_settings;
        struct termios new_settings;

        tcgetattr(STDIN_FILENO, &old_settings);

        new_settings = old_settings;

        new_settings.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(
            STDIN_FILENO,
            TCSANOW,
            &new_settings
        );

        char key = getchar();

        tcsetattr(
            STDIN_FILENO,
            TCSANOW,
            &old_settings
        );

        return key;
    }


    // =========================================
    // SPEED SETTINGS
    // =========================================

    double speed_ = 2.0;

    double turn_speed_ = 2.0;


    // =========================================
    // ROS
    // =========================================

    rclcpp::Publisher<
        std_msgs::msg::Float64MultiArray
    >::SharedPtr publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
};


int main(
    int argc,
    char *argv[])
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<TeleopNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}