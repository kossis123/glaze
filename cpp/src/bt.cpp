#include <rclcpp/rclcpp.hpp>
#include <behaviortree_cpp/bt_factory.h>
#include "/home/iamironman/ros2/src/cpp/include/cpp/bt.hpp"

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<Node>("CounterNode");
const std::string xml_path = "/home/iamironman/ros2/src/cpp/xml/bt.xml";
    auto tree = factory.createTreeFromFile(xml_path);

    BT::NodeStatus status = BT::NodeStatus::RUNNING;
    while (status == BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status = tree.tickOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (status == BT::NodeStatus::SUCCESS)
        RCLCPP_INFO(rclcpp::get_logger("main"), "Tree finished with SUCCESS");
    else
        RCLCPP_INFO(rclcpp::get_logger("main"), "Tree finished with FAILURE");

    rclcpp::shutdown();
    return 0;
}