#include<rclcpp/rclcpp.hpp>
#include</home/iamironman/ros2/src/cpp/include/cpp/kossis.hpp>
#include<behaviortree_cpp/bt_factory.h>
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    BT::BehaviorTreeFactory factory;
    factory.registerNodeType<hero>("CounterNode");
    const std::string xml="/home/iamironman/ros2/src/cpp/xml/bt.xml";
    auto tree=factory.createTreeFromFile(xml);
    BT::NodeStatus status=BT::NodeStatus::RUNNING;
    
    while(status==BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status=tree.tickOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
    if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(rclcpp::get_logger("main"),"Tree finished successfully");
    }
    else if(status==BT::NodeStatus::FAILURE)
    {
        RCLCPP_INFO(rclcpp::get_logger("main"),"Failed");
    }

    
    rclcpp::shutdown();
}