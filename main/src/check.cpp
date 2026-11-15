#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "ros_can/can.hpp"
#include "ros_can/motor.hpp"
#include "robotlib/kinematics/omniwheel.hpp"


class OmniNode : public rclcpp::Node
{
private:

    
    can can1;

    
    motor motor1;
    motor motor2;
    motor motor3;
    motor motor4;

    
    OmniwheelKinematics wheels;


    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;

public:

    OmniNode()
    : Node("omni_node"),
      can1("can0"),
      motor1(can1, 1, "cg"),
      motor2(can1, 2, "cg"),
      motor3(can1, 3, "cg"),
      motor4(can1, 4, "cg"),
      wheels(0.2, 0.3)
    {
        
        can1.start_rx();

        
        motor* motors[4] = {
            &motor1, &motor2, &motor3, &motor4
        };

        for (auto m : motors)
        {
            m->exit_mit();
            m->enter_mit();
            m->set_mech_zero();
            m->enter_mit();
        }


        sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(
                &OmniNode::velocity_callback,
                this,
                std::placeholders::_1
            )
        );

        RCLCPP_INFO(this->get_logger(), "Omni node started!");
    }

    void velocity_callback(
        const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        
        Twist twist;

        twist.vx = msg->linear.x;
        twist.vy = msg->linear.y;
        twist.w  = msg->angular.z;

        
        float motor_omega[4];

        wheels.get_motor_omega(twist, motor_omega);


        motor1.cmd(0, motor_omega[0], 0, 1.0, 0);
        motor2.cmd(0, motor_omega[1], 0, 1.0, 0);
        motor3.cmd(0, motor_omega[2], 0, 1.0, 0);
        motor4.cmd(0, motor_omega[3], 0, 1.0, 0);
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<OmniNode>());

    rclcpp::shutdown();

    return 0;
}