#include</home/iamironman/ros2/src/cpp/include/cpp/again.hpp>

start::start(const std::string &name, const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_):
BT::StatefulActionNode(name,config,node):
node(node_)
{
        C_=rclcpp_action::create_client<intefaces::action::Node>("/server");
        
}
BT::portsList start::providedPorts()
{
    return{
        BT::InputPort<float>("target_x");
        BT::InputPort<float>("target_y");
    };
}
BT::NodeStatus start::onStart()
{
    move_x=getInput<float>("target_x").value();
    move_y=getInput<float>("target_y").value();
    
    return BT::NodeStatus::RUNNING;

}
BT::NodeStatus start::onRunning()
{
    if(!wait_for_action_servcie(std::chrono::milliseconds(0)))
    {
        RCLCPP_WARN(node->get_logger(),"Waiting for server");
        return BT::NodeStatus::RUNUNING;
    }
    if(!goal_sent)
    {
        send_options_.goal_response_callback=[this](std::shared_ptr<rclcpp_aciton<interfaces::action::Node>> goal_handle){
            if(!goal_handle)
            {
                 goal_rejected_=true;
                 RCLCPP_INFO(node->get_logger(),"rejectec");

            }
            else
            {
                RCLCPP_INFO(node->get_logger(),"goal_accepted");
            }
        };
        send_options_.feedback_callback=[this](std::shared_ptr<rclcpp_action<interfaces::action::Node>> , std::shared_ptr<interfaces::action::Node> )
        {

        };
        send_options_.result_callback=[this](rclcpp_action::ClientGoalHandle<interfaces::action::Node>WrappedResult result)
        {
            goal_done=true;
                        
        };
    }
}

