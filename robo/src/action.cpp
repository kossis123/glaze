#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/check.hpp>
#include<thread>
#include <memory>
class node:public rclcpp::Node
{
    public:
    node():Node("koshish"){
    s_=rclcpp_action::create_server<interfaces::action::Check>(this,
        "/server",
        std::bind(&node::handle_goal,this,std::placeholders::_1,std::placeholders::_2),
        std::bind(&node::handle_cancel,this,std::placeholders::_1),
        std::bind(&node::handle_execution,this,std::placeholders::_1)
    );
    RCLCPP_INFO(
        this->get_logger(),
        "Action server started. Waiting for goal..."
    );
}
    private:
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &uuid,std::shared_ptr<const interfaces::action::Check::Goal> goal)
    {
        (void)uuid;
        RCLCPP_INFO(this->get_logger(),"the goal is recieved %f %f ",goal->target_x,goal->target_y);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Check>> goal_handle)
    {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(),"the goal is canceled");
        return rclcpp_action::CancelResponse::ACCEPT;
    }
    void handle_execution (const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Check>> goal_handle)
    {
        std::thread(&node::callback,this,goal_handle).detach();
    }
    void callback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Check>> goal_handle)
    {

        auto goal=goal_handle->get_goal();
        auto result=std::make_shared<interfaces::action::Check::Result>();
        result->sum=goal->target_x + goal->target_y;
        result->success=true;
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(),"Goal succeded");

    }
    rclcpp_action::Server<interfaces::action::Check>::SharedPtr s_;
};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}