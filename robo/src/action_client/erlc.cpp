#include </home/iamironman/ros2/src/robo/include/robo/final.hpp>

robot::robot(const std::string &name,const BT::NodeConfig &config,std::shared_ptr<rclcpp::Node> node_):
BT::StatefulActionNode(name,config),
node(node_)
{
    c_=rclcpp_action::create_client<interfaces::action::Er>(
        node,
        "server"
    );

}

BT::PortsList robot::providedPorts()
{
    return
    {
        BT::InputPort<float>("x"),
        BT::InputPort<float>("y")
    };
}

BT::NodeStatus robot::onStart()
{
    goal_sent = false;
    goal_rejected = false;
    goal_accepted = false;
    goal_done = false;
    goal_succeed = false;
    goal_handle_.reset();
    value=getInput<float>("x").value();
    value2=getInput<float>("y").value();
    return BT::NodeStatus::RUNNING;
}

BT::NodeStatus robot::onRunning()
{
    if(!goal_sent){
    if(!c_->wait_for_action_server(std::chrono::milliseconds(0)))
    {
        RCLCPP_INFO(node->get_logger(),"no server detected");
        return BT::NodeStatus::RUNNING;
    }
    goal.x=value;
    goal.y=value2;
    send_.goal_response_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Er>> goal_handle)
    {
        if(!goal_handle)
        {
             RCLCPP_INFO(node->get_logger(),"no server detected");
             goal_rejected=true;
        }
        else
        {
            goal_handle_=goal_handle;
            goal_accepted=true;
            RCLCPP_INFO(node->get_logger(),"no server detected");
        }
    };
    send_.feedback_callback=[this](std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Er>> ,std::shared_ptr<const interfaces::action::Er::Feedback>)
    {

    };

    send_.result_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Er>::WrappedResult result)
    {
        if(result.result->success)
        {
            RCLCPP_INFO(node->get_logger(),"success");
            goal_done=true;
            goal_succeed=result.result->success;

        }

        else
        {
            goal_done=true;
            goal_succeed=false;
        
        }
    };
    goal_sent=true;
    c_->async_send_goal(goal,send_);
    }

    if(goal_rejected)
    {
        RCLCPP_INFO(node->get_logger(),"The tree failed");
        return BT::NodeStatus::FAILURE;
    }

    if(goal_done)
    {
        if(goal_succeed)
        {
            return BT::NodeStatus::SUCCESS;
        }
        
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::RUNNING;
}

void robot::onHalted()
{
    if(goal_handle_)
    {
        c_->async_cancel_goal(goal_handle_);
        RCLCPP_INFO(node->get_logger(),"The goal is canceled");
    }

}