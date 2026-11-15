#include <rclcpp_action/rclcpp_action.hpp>
#include <rclcpp/rclcpp.hpp>
#include <interfaces/action/er.hpp>
#include <thread>


class node:public rclcpp::Node
{
    public:
    node():Node("koshish")
    {
        se_=rclcpp_action::create_server<interfaces::action::Er>(
            this,
            "server",
            std::bind(&node::handle_goal,this,std::placeholders::_1,std::placeholders::_2),
            std::bind(&node::handle_cancel,this,std::placeholders::_1),
            std::bind(&node::handle_execute,this,std::placeholders::_1)
        );
    }
    private:
    
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &uuid,std::shared_ptr<const interfaces::action::Er::Goal > goal)
    {

        (void)uuid;
        RCLCPP_INFO(this->get_logger(),"The goal is received. Goal=%f ,%f",goal->x,goal->y);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;

    }

    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<rclcpp_action::ServerGoalHandle< interfaces::action::Er>> goal_handle)
    {
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Er>> goal_handle)
    {
        std::thread(&node::execute,this,goal_handle).detach();

    }
    void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Er>> goal_handle)
    {
        auto goal=goal_handle->get_goal();
        auto result=std::make_shared<interfaces::action::Er::Result>();
        result->sum=goal->x + goal->y;
        result->success=true;
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(),"Goal succeded");
    }
    rclcpp_action::Server<interfaces::action::Er>::SharedPtr se_;

};
int main(int argc, char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}