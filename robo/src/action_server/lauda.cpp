#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <interfaces/action/er.hpp>

class node:public rclcpp::Node
{
    public:
    node():Node("koshish")
    {
        s_=rclcpp_action::create_server<interfaces::action::Er>(
            this,
            "server",
            std::bind(&node::handle_accept,this,std::placeholders::_1,std::placeholders::_2),
            std::bind(&node::handle_cancel,this,std::placeholders::_1),
            std::bind(&node::execute,this,std::placeholders::_1)
        );

    }
    private:
    rclcpp_action::GoalResponse handle_accept(const rclcpp_action::GoalUUID &uuid,std::shared_ptr<const interfaces::action::Er::Goal> goal)
    {
        (void)uuid;
        (void)goal;
        RCLCPP_INFO(this->get_logger(),"the goals are %f %f",goal->x,goal->y);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Er>> goal_handle)
    {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(),"goal canceled");
        return rclcpp_action::CancelResponse::ACCEPT;

    }

    void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Er>> goal_handle)
    {   
        std::thread(&node::ex,this,goal_handle).detach();
    }

    void ex(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Er>> goal_handle)
    {

        auto goal=goal_handle->get_goal();
        auto result=std::make_shared<interfaces::action::Er::Result>();
        result->sum=goal->x + goal->y;
        result->success=true;
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(),"the result is calculated %f", result->sum);

    }
    rclcpp_action::Server<interfaces::action::Er>::SharedPtr s_;
    
};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
    return 0;
}