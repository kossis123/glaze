#include<behaviortree_cpp/action_node.h>
#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/check.hpp>

class start: public BT::StatefulActionNode
{

    public:
    start(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_);
    static BT::PortsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;
    
    private:
    rclcpp::Node::SharedPtr node;
    rclcpp_action::Client<interfaces::action::Check>::SharedPtr c_;
    interfaces::action::Check::Goal goal;
    std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Check>> goal_handle_;
    rclcpp_action::Client<interfaces::action::Check>::SendGoalOptions send_;
    std::atomic<bool>goal_done{false};
    std::atomic<bool>goal_succeded{false};
    std::atomic<bool>goal_rejected{false};
    std::atomic<bool>goal_sent{false};
    std::atomic<bool>goal_accepted{false};
    int x;
    int y;


};