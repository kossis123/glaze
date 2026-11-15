#ifndef HERO_HPP
#define HERO_HPP
#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/action_node.h>
#include<interfaces/srv/node.hpp>
#include<behaviortree_cpp/bt_factory.h>
class service:public BT::StatefulActionNode
{
    public:
    service(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_);
    static BT::PortsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;
    
    private:
    rclcpp::Node::SharedPtr node;
    rclcpp::Client<interfaces::srv::Node>::SharedPtr c_;
    rclcpp::Client<interfaces::srv::Node>::SharedFuture future;
};
#endif