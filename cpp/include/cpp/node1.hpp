#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/action_node.h>
class node:public BT::StatefulActionNode
{
    public:
    node(const std::string &name,const BT::NodeConfig &config):BT::StatefulActionNode(name,config)
    {}
    static BT::PortsList providedPorts()
    {
        return{
            BT::OutputPort<int>("count")
        };
    }
    BT::NodeStatus onStart() override
    {
        count_=0;
        RCLCPP_INFO(rclcpp::get_logger("node"),"counter started");
        return BT::NodeStatus::RUNNING;
        
    }
    BT::NodeStatus onRunning() override
    {
        count_++;
        if(count_>5)
        {
            RCLCPP_INFO(rclcpp::get_logger("node"),"The first node is completed");
            return BT::NodeStatus::SUCCESS;
        }
        else
        {
            RCLCPP_INFO(rclcpp::get_logger("node"),"counted %d",count_);
            return BT::NodeStatus::RUNNING;
        }
    }
    void onHalted() override
    {
        RCLCPP_INFO(rclcpp::get_logger("node"),"Failed");
    }
    private:
    int count_=0;
};