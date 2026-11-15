#include<behaviortree_cpp/action_node.h>
#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/check.hpp>
#include</home/iamironman/ros2/src/robo/include/robo/new.hpp>

start::start(const std::string &name ,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_):
BT::StatefulActionNode(name,config),
node(node_)
{
    c_=rclcpp_action::create_client<interfaces::action::Check>(node,"/server");
}
BT::PortsList start::providedPorts()
{
    return
    {
        BT::InputPort<int>("x"),
        BT::InputPort<int>("y")
    };
}
BT::NodeStatus start::onStart()
{
    
    goal_done=false;
    goal_sent=false;
    goal_succeded=false;
    goal_rejected=false;
    goal_accepted=false;
    goal_handle_.reset();
    x=getInput<int>("x").value();
    y=getInput<int>("y").value();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus start::onRunning()
{
    if(!goal_sent)
    {
        goal.target_x=x;
        goal.target_y=y;
        if(!c_->wait_for_action_server(std::chrono::milliseconds(0)))
            {
                RCLCPP_WARN(node->get_logger(),"No server found!!!!");
                return BT::NodeStatus::RUNNING;
            }
        send_.goal_response_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Check>> goal_handle){
                if(!goal_handle)
                {
                    RCLCPP_INFO(node->get_logger(),"no goal found");
                    goal_rejected=true;
                    
                }
                else
                {
                    goal_accepted=true;
                    goal_handle_=goal_handle;
                }
        };
        send_.feedback_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Check>> ,std::shared_ptr<const interfaces::action::Check::Feedback> ){

        };
        send_.result_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Check>::WrappedResult result)
        {
            goal_succeded=result.result->success;
            RCLCPP_INFO(node->get_logger(),"%f",result.result->sum);
            goal_done=true;
        };
        c_->async_send_goal(goal,send_);
        goal_sent=true;
        return BT::NodeStatus::RUNNING;
    }
    if(goal_rejected)
    {
        RCLCPP_INFO(node->get_logger(),"Goal rejected");
        return BT::NodeStatus::FAILURE;
    }
    if(goal_done)
    {
        if(goal_succeded)
        {
        RCLCPP_INFO(node->get_logger(),"Goal reached");
        return BT::NodeStatus::SUCCESS;   
        }
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::RUNNING;
}

void start::onHalted()
{

    if(goal_handle_)
    {
        c_->async_cancel_goal(goal_handle_);
        RCLCPP_WARN(node->get_logger(), "canceled");
    
    }
    
}