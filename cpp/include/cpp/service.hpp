#include</home/iamironman/ros2/src/cpp/include/cpp/hero.hpp>
using namespace std::chrono_literals;
class node:public BT::StatefulActionNode
{
    public:
    node(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_)
    :BT::StatefulActionNode(name,config)
    :node(node_)
    {
        RCLCPP_INFO(node->get_logger(),"ready to send");
        c_=node->create_client<interfaces::srv::Node>("/server");
    
    }
    static BT::PortsList providedPorts()
    {

        BT::InputPort<float>("target_x");
        BT::InputPort<float>("target_y");

    }
    BT::NodeStatus onStart() override
    {

        if(!client->wait_for_service(0s))
        {
            RCLCPP_WARN(node->get_logger()," Waiting For Connection ");

        }
        auto data=std::make_shared<interfaces::srv::Data::Request>
        auto future=c_->send_async_goal();

        return BT::NodeStatus RUNNING;

    }
    
    private:
    rclcpp::Node::SharedPtr node;
    rclcpp::Client<interfaces::srv::Node>::SharedPtr c_;
    rclcpp::TimerBase::SharedPtr t_;

}