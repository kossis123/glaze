#include<behaviortree_cpp/behaviortree_cpp.hpp>
#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/node.hpp>

class start:public BT::StatefulActionNode
{
    public:
    start(const std::string &name,const BT::Nodeconfig &config,rclcpp::Node::SharedPtr node_);
    static BT::portsList providedPorts();
    BT::NodeStatus onStart() override;
    BT::NodeStatus onRunning() override;
    void onHalted() override;

    private:
    std::atomic<bool>goal_sent_{false};
    std::atomic<bool>goal_succeded_{false};
    std::atomic<bool>goal_failed{false};
    std::atomic<bool>goal_rejected{false};
    std::atomic<bool>goal_done{false};
    rclcpp::Node::SharedPtr node;
    rclcpp_action::Client<interfaces::action::Node>::SharedPtr c_;
    rclcpp_action::Client<interfaces::action::Node>::SendGoalOptions send_options_;
    std::shared_ptr<rclcpp_action::ClientGoalHandle<interfaces::action::Node>> goal_handle_;
    float movex;
    float movey;




};