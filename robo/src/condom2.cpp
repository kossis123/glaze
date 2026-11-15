#include <rclcpp_action/rclcpp_action.hpp>
#include <rclcpp/rclcpp.hpp>
#include <interfaces/action/er.hpp>


using namespace std::chrono_literals;

class node : public rclcpp::Node
{
public:
    node() : Node("Koshish")
    {
        c_ = rclcpp_action::create_client<interfaces::action::Er>(
            this,
            "server"
        );

        t_ = this->create_wall_timer(
            1s,
            std::bind(&node::callback, this)
        );
    }

private:

    void callback()
    {
        t_->cancel();

        auto goal = std::make_shared<interfaces::action::Er::Goal>();

        goal->x = 10.0;
        goal->y = 12.0;

        send_goal_.goal_response_callback =
            [this](const std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Er>> goal_handle)
        {
            if (!goal_handle)
            {
                RCLCPP_WARN(this->get_logger(), "Goal rejected");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(), "Goal accepted");
            }
        };

        send_goal_.feedback_callback =
            [this](
                const std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Er>> goal_handle,
                const std::shared_ptr<const interfaces::action::Er::Feedback> feedback)
        {
            (void)goal_handle;
            (void)feedback;
        };

        send_goal_.result_callback =
            [this](const rclcpp_action::ClientGoalHandle<interfaces::action::Er>::WrappedResult result)
        {
            RCLCPP_INFO(this->get_logger(), "%f", result.result->sum);
        };

        c_->async_send_goal(goal, send_goal_);
    }

    rclcpp::TimerBase::SharedPtr t_;

    rclcpp_action::Client<interfaces::action::Er>::SharedPtr c_;

    rclcpp_action::Client<interfaces::action::Er>::SendGoalOptions send_goal_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<node>());

    rclcpp::shutdown();

    return 0;
}