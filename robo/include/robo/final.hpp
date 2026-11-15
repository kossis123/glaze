#ifndef FINAL_HPP
#define FINAL_HPP
#include <behaviortree_cpp/action_node.h>
#include <behaviortree_cpp/bt_factory.h>
#include <rclcpp/rclcpp.hpp>
#include <interfaces/action/er.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

class robot:public BT::StatefulActionNode
{
public:

    robot(const std::string &name, const BT::NodeConfig &config,std::shared_ptr<rclcpp::Node> node_);
    static BT::PortsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

interfaces::action::Er::Goal goal;
   

private:
    rclcpp_action::Client<interfaces::action::Er>::SharedPtr c_;
    rclcpp_action::Client<interfaces::action::Er>::SendGoalOptions send_;
    rclcpp::Node::SharedPtr node;
    float value;
    float value2;
        std::atomic<bool>goal_done{false};
    std::atomic<bool>goal_succeed{false};
    std::atomic<bool>goal_rejected{false};
    std::atomic<bool>goal_sent{false};
    std::atomic<bool>goal_accepted{false};
    std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Er>> goal_handle_;
};

#endif