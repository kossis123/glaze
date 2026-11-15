#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/action_node.h>
#include<std_msgs/msg/string.hpp>
class Node:public BT::StatefulActionNode
{
    public:
    Node(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_):
    BT::StatefulActionNode(name,config),
    node(node_)
    {

    }
    static BT::PortsList providedPorts()
    {
        return {

        };
    }
    BT::NodeStatus onStart() override
    {
        pub_=node->create_publisher<std_msgs::msg::String>("/msg",10);
        RCLCPP_INFO(node->get_logger(),"Publisher created and ready to publish");
        return BT::NodeStatus::RUNNING;
    }
    BT::NodeStatus onRunning() override
    {
        count_++;
        std_msgs::msg::String msg;
        msg.data="hello world";
        pub_->publish(msg);
        if(count_==3)
        {
            return BT::NodeStatus::SUCCESS;
        }
        else
        return BT::NodeStatus::RUNNING;
    }
    void onHalted() override
    {
        RCLCPP_INFO(node->get_logger(),"Halted");
    }
    private:
    rclcpp::Node::SharedPtr node;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    int count_=0;
};