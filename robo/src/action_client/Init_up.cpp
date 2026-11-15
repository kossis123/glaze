#include "/home/iamironman/ros2/src/robo/include/robo/init2.hpp"

end::end(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_):
BT::StatefulActionNode(name,config),
node(node_)
{
    c_=rclcpp_action::create_client<interfaces::action::Init>(node,
        "/server");
    
    sub_=node->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states",
            10,
            std::bind(&end::callback,this,std::placeholders::_1)
        );
    
}

BT::PortsList end::providedPorts()
{
    return{
        BT::InputPort<float>("value2")  
    };
}

BT::NodeStatus end::onStart()
{

    goal_done=false;
    goal_accepted=false;
    goal_sent=false;
    goal_rejected=false;
    goal_succeed=false;
    goal_handle_.reset();

    value=getInput<float>("value2").value();
    return BT::NodeStatus::RUNNING;

}

BT::NodeStatus end::onRunning()
{

    if(!goal_sent)
    {

        goal.value.push_back(value);
        if(!c_->wait_for_action_server(std::chrono::milliseconds(0)))
        {
            RCLCPP_WARN(node->get_logger(),"No connection till now");
        }
        send_.goal_response_callback=[this]( std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Init>> goal_handle){
            goal_handle_=goal_handle;
            if(!goal_handle){
                
                RCLCPP_INFO(node->get_logger(),"No goal sent");
                goal_rejected=true;
            }
            else
            {
                goal_accepted=true;
                RCLCPP_INFO(node->get_logger(),"Goal reached");
            }
        };

        send_.feedback_callback=[this]( std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Init>> goal_handle, std::shared_ptr<const interfaces::action::Init::Feedback> feedback)
        
        {
            (void)goal_handle;
            // RCLCPP_INFO(node->get_logger(),"Pose: %f",feedback->pose[0]);
            (void)feedback;
        };

        send_.result_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Init>::WrappedResult result)
        {

            if(result.result->success){

            RCLCPP_INFO(node->get_logger(),"The goal is reached");
            goal_done=true;
            goal_succeed=result.result->success;
            
            }
            
            else{

            RCLCPP_INFO(node->get_logger(),"The goal is not reached");
            goal_done=true;
            goal_succeed=false;

            }

        };

        goal_sent=true;
        c_->async_send_goal(goal,send_);
        return BT::NodeStatus::RUNNING;
    }

    if(goal_rejected)
    {
        RCLCPP_INFO(node->get_logger(),"The tree failed");
        return BT::NodeStatus::FAILURE;
    }
    if(goal_done)
    {   
        
        if(end::pose>-0.28)
        {
            return BT::NodeStatus::RUNNING;
        }
        if(goal_succeed)
        {
            RCLCPP_INFO(node->get_logger(),"The goal is reached");
            return BT::NodeStatus::SUCCESS;
        }

            return BT::NodeStatus::FAILURE;
        
    }
    return BT::NodeStatus::RUNNING;
}

void end::onHalted()
{
    if(goal_handle_)
    {
        c_->async_cancel_goal(goal_handle_);
        RCLCPP_INFO(node->get_logger(),"The goal is canceled");
    }

}

void end::callback(sensor_msgs::msg::JointState msg)
{

    if (msg.position.size() > 3)
    {
        pose = msg.position[3];
    }
}