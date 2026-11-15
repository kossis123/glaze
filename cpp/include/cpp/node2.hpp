#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/action_node.h>
class Node:public BT::StatefulActionNode
{
    public:
    Node(const std::string &name,const BT::NodeConfig &config):BT::StatefulActionNode(name,config)
    {}
    static BT::PortsList providedPorts()
    {
        return{
            BT::OutputPort<int>("counter")
        };
    }
    BT::NodeStatus onStart() override
    {
        count_=0;
        RCLCPP_INFO(rclcpp::get_logger("Node"),"counter started");
        return BT::NodeStatus::RUNNING;
        
    }
    BT::NodeStatus onRunning() override
    {
        count_++;
        if(count_>5)
        {
            RCLCPP_INFO(rclcpp::get_logger("Node"),"The second node is completed");
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            RCLCPP_INFO(rclcpp::get_logger("Node"),"counted %d",count_);
            return BT::NodeStatus::RUNNING;
        }
    }
    void onHalted() override
    {
        RCLCPP_INFO(rclcpp::get_logger("Node"),"Failed");
    }
    private:
    int count_=0;
};