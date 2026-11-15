#include <rclcpp/rclcpp.hpp>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>

using namespace std::chrono_literals;

class ArmController : public rclcpp::Node
{
public:
    ArmController() : Node("arm_controller")
    {
        p_ = this->create_publisher<trajectory_msgs::msg::JointTrajectory>(
            "/arm/joint_trajectory", 10
        );
        t_ = this->create_wall_timer(
            500ms,
            std::bind(&ArmController::callback, this)
        );
    }

private:
    void callback()
    {
        t_->cancel();
        trajectory_msgs::msg::JointTrajectory msg;

        msg.joint_names = {
            "Joint_1",
            "Joint_2",
            "Joint_3",
            "Joint_4",
            "Joint_5",
            "Joint_6",
            "Joint_Gripper"
        };

        trajectory_msgs::msg::JointTrajectoryPoint point;
        point.positions = {1.0, 1.0, 0.54, 0.7, 0.9, 1.2, 1.5};
        point.time_from_start = rclcpp::Duration::from_seconds(0.3);

        msg.points.push_back(point);
        p_->publish(msg);

        RCLCPP_INFO(this->get_logger(), "Published joint trajectory");
    }

    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr p_;
    rclcpp::TimerBase::SharedPtr t_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ArmController>());
    rclcpp::shutdown();
    return 0;
}