#include</home/iamironman/ros2/src/cpp/include/cpp/turtle_start.hpp>

move_start::move_start(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_):
BT::StatefulActionNode(name,config),
node(node_)
{
c_=rclcpp_action::create_client<interfaces::action::Node>(node,"/server");
}

BT::PortsList move_start::providedPorts()
{
    return{
    BT::InputPort<float>("target_x"),
    BT::InputPort<float>("target_y")
    };
}

BT::NodeStatus move_start::onStart()
{
    move_x=getInput<float>("target_x").value();
    move_y=getInput<float>("target_y").value();
    
    return BT::NodeStatus::RUNNING;
}
BT::NodeStatus move_start::onRunning()
{
    if(!goal_sent_)
    {
        interfaces::action::Node::Goal goal;
        if(!c_->wait_for_action_server(std::chrono::milliseconds(0)) )
        {
            RCLCPP_WARN(node->get_logger(),"Waiting For Connection");
            return BT::NodeStatus::RUNNING;
        }
        goal.target_x=move_x;
        goal.target_y=move_y;
        send_options_.goal_response_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Node>::SharedPtr goal_handle)
        {
            if(!goal_handle)
            {
                RCLCPP_WARN(node->get_logger(),"Response Failed");
                goal_rejected_=true;
            }
            else
            {
                goal_handle_=goal_handle;
                goal_accepted_=true;
                
            }
        };
        send_options_.feedback_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Node>::SharedPtr,
            std::shared_ptr<const interfaces::action::Node::Feedback> )
        {

        };
        send_options_.result_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Node>::WrappedResult result)
        {
            goal_done_=true;
            goal_succeded_=result.result->success;
            RCLCPP_INFO(node->get_logger(),"distance travelled %f",result.result->distance);

        };
        c_->async_send_goal(goal,send_options_);
        goal_sent_=true;
        return BT::NodeStatus::RUNNING;
        
    }
    if(goal_rejected_)
    {
        RCLCPP_INFO(node->get_logger(),"Goal rejected");
        return BT::NodeStatus::FAILURE;
    }
    if(goal_done_)
    {
        if(goal_succeded_)
        {
            RCLCPP_INFO(node->get_logger(),"Goal succeded");
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::RUNNING;

}
void move_start::onHalted()
{
    if(goal_handle_)
    {
        c_->async_cancel_goal(goal_handle_);
        RCLCPP_WARN(node->get_logger(), "canceled");
    
    }
    
}