#include <behaviortree_cpp/action_node.h>
#include <rclcpp/rclcpp.hpp>

class Node : public BT::StatefulActionNode
{
public:
    Node(const std::string& name, const BT::NodeConfig& config)
        : BT::StatefulActionNode(name, config) {}

    static BT::PortsList providedPorts()
    {
        return {
            BT::InputPort<int>("target_value"),
            BT::OutputPort<int>("count")
        };
    }

    BT::NodeStatus onStart() override
    {
        count_ = 0;
        if (!getInput<int>("target_value", target_value_))
        {
            throw BT::RuntimeError("missing input port: target_value");
        }
        RCLCPP_INFO(rclcpp::get_logger("Node"), "Node starting, target: %d", target_value_);
        return BT::NodeStatus::RUNNING;
    }

    BT::NodeStatus onRunning() override
    {
        count_++;
        setOutput("count", count_);
        RCLCPP_INFO(rclcpp::get_logger("Node"), "count is now %d", count_);

        if (count_ >= target_value_)
        {
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::RUNNING;
    }

    void onHalted() override 
    {
        RCLCPP_INFO(rclcpp::get_logger("Node"), "halted");
    }

private:
    int count_ = 0;
    int target_value_ = 0; 
};