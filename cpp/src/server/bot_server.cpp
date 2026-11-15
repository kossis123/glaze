#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <interfaces/action/node.hpp>
#include <turtlesim/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <thread>
#include <mutex>
#include <cmath>

class NodeServer : public rclcpp::Node
{
public:
    NodeServer() : Node("node_server")
    {
        using namespace std::placeholders;

        server_ = rclcpp_action::create_server<interfaces::action::Node>(
            this,
            "/server",
            std::bind(&NodeServer::handle_goal, this, _1, _2),
            std::bind(&NodeServer::handle_cancel, this, _1),
            std::bind(&NodeServer::handle_accept, this, _1)
        );

        sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            10,
            std::bind(&NodeServer::pose_callback, this, _1)
        );

        pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel",
            10
        );
    }

private:

    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const interfaces::action::Node::Goal> goal)
    {
        (void)uuid;

        RCLCPP_INFO(
            this->get_logger(),
            "Goal received: x=%f y=%f",
            goal->target_x,
            goal->target_y
        );

        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    
    rclcpp_action::CancelResponse handle_cancel(
        std::shared_ptr<
            rclcpp_action::ServerGoalHandle<interfaces::action::Node>
        >)
    {
        RCLCPP_WARN(this->get_logger(), "Cancel request accepted");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    
    void handle_accept(
        std::shared_ptr<
            rclcpp_action::ServerGoalHandle<interfaces::action::Node>
        > goal_handle)
    {
        std::thread(&NodeServer::execute, this, goal_handle).detach();
    }

    
    void execute(
        std::shared_ptr<
            rclcpp_action::ServerGoalHandle<interfaces::action::Node>
        > goal_handle)
    {
        
        auto goal = goal_handle->get_goal();
        rclcpp::Rate rate(10);

        geometry_msgs::msg::Twist cmd;
        
        while (rclcpp::ok())
        {
            turtlesim::msg::Pose pose;

            {
                std::lock_guard<std::mutex> lock(mutex_);
                pose = current_pose_;
            }

            double dx = goal->target_x - pose.x;
            double dy = goal->target_y - pose.y;

            double distance = std::sqrt(dx * dx + dy * dy);

            double desired_angle = std::atan2(dy, dx);
            double error = desired_angle - pose.theta;
            error = std::atan2(std::sin(error), std::cos(error));

            
            auto feedback =
                std::make_shared<interfaces::action::Node::Feedback>();

            feedback->current_angle = pose.theta;
            goal_handle->publish_feedback(feedback);

            if (goal_handle->is_canceling())
            {

                cmd.linear.x = 0.0;
                cmd.angular.z = 0.0;
                pub_->publish(cmd);

                auto result =
                    std::make_shared<interfaces::action::Node::Result>();

                result->success = false;
                result->distance = distance;

                goal_handle->canceled(result);
                return;

            }
            
            if (distance > 0.1)
            {
                cmd.linear.x = 2.0;
                cmd.angular.z = 4.0 * error;
            }
            else
            {
                cmd.linear.x = 0.0;
                cmd.angular.z = 0.0;
                pub_->publish(cmd);

                auto result =
                    std::make_shared<interfaces::action::Node::Result>();

                result->success = true;
                result->distance = distance;

                goal_handle->succeed(result);
                return;
            }

            pub_->publish(cmd);
            rate.sleep();
        }
    }

    
    void pose_callback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        current_pose_ = *msg;
    }

private:

    rclcpp_action::Server<interfaces::action::Node>::SharedPtr server_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;

    turtlesim::msg::Pose current_pose_;
    std::mutex mutex_;

};
int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<NodeServer>());
    rclcpp::shutdown();
    return 0;
}