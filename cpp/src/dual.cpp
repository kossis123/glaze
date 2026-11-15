#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include</home/iamironman/ros2/src/cpp/include/cpp/node1.hpp>
#include</home/iamironman/ros2/src/cpp/include/cpp/node2.hpp>
int main(int argc,char *argv[])
{

    rclcpp::init(argc,argv);
    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<node>("Enter");
    factory.registerNodeType<Node>("Exit");
    const std::string xml="/home/iamironman/ros2/src/cpp/xml/last.xml";
    auto tree=factory.createTreeFromFile(xml);
    auto status=BT::NodeStatus::RUNNING;
    
    while(status==BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status=tree.tickOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(rclcpp::get_logger("main"),"TreeFinished");
    }

    rclcpp::shutdown();

}