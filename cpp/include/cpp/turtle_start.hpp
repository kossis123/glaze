#include<behaviortree_cpp/action_node.h>
#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/node.hpp>

class move_start:public BT::StatefulActionNode
{
    public:
    move_start(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_);
    static BT::PortsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

    private:
    rclcpp::Node::SharedPtr node;
    rclcpp_action::Client<interfaces::action::Node>::SharedPtr c_;
    std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Node>> goal_handle_;
    rclcpp_action::Client<interfaces::action::Node>::SendGoalOptions send_options_;
    std::atomic<bool>goal_succeded_{false};
    std::atomic<bool>goal_sent_{false};
    std::atomic<bool>goal_rejected_{false};
    std::atomic<bool>goal_done_{false};
    std::atomic<bool>goal_accepted_{false};
    float move_x;
    float move_y;

};