#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/check.hpp>
#include<thread>
#include <memory>

using namespace std::chrono_literals;
class node: public rclcpp::Node
{
    public:
    node():Node("koshish")
    {
        c_=rclcpp_action::create_client<interfaces::action::Check>(
            this,
            "/server"
        );
        t_=this->create_wall_timer(1s,
        std::bind(&node::callback,this));
    }
    private:
    void callback()
    {
        t_->cancel();
    if (!c_->wait_for_action_server(1s))
    {
        RCLCPP_ERROR(this->get_logger(), "action server not available");
        return;
    }
        rclcpp_action::Client<interfaces::action::Check>::SendGoalOptions send_options_;
        interfaces::action::Check::Goal goal;
        goal.target_x=12;
        goal.target_y=12;
        send_options_.goal_response_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Check>> goal_handle){
            if(!goal_handle)
            {
                RCLCPP_INFO(this->get_logger(),"not gettting any response");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(),"server connected");
            }
            
        };
        send_options_.feedback_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Check>>,std::shared_ptr<const interfaces::action::Check::Feedback>){

        };
        send_options_.result_callback=[this](const rclcpp_action::ClientGoalHandle<interfaces::action::Check>::WrappedResult result){
                RCLCPP_INFO(this->get_logger(),"the result is %f",result.result->sum);
        };
        c_->async_send_goal(goal,send_options_);
    }
    rclcpp_action::Client<interfaces::action::Check>::SharedPtr c_;
    rclcpp::TimerBase::SharedPtr t_;

};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}