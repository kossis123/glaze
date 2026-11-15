#include<behaviortree_cpp/action_node.h>
#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/init.hpp>
#include<sensor_msgs/msg/joint_state.hpp>


class end: public BT::StatefulActionNode
{

    public:
    end(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_);
    static BT::PortsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;
       void callback(
        const sensor_msgs::msg::JointState msg);

    
    private:
    rclcpp::Node::SharedPtr node;
    rclcpp_action::Client<interfaces::action::Init>::SharedPtr c_;
    interfaces::action::Init::Goal goal;
    std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Init>> goal_handle_;
    rclcpp_action::Client<interfaces::action::Init>::SendGoalOptions send_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr sub_;
    std::atomic<bool>goal_done{false};
    std::atomic<bool>goal_succeed{false};
    std::atomic<bool>goal_rejected{false};
    std::atomic<bool>goal_sent{false};
    std::atomic<bool>goal_accepted{false};
    float value;
    float pose;



};