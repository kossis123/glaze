#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/bt_factory.h>
#include</home/iamironman/ros2/src/cpp/include/cpp/node.hpp>
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    auto ros_node=std::make_shared<rclcpp::Node>("bt");
    BT::BehaviorTreeFactory factory;
    factory.registerBuilder<Node>("Runner",
        [ros_node](const std::string &name,const BT::NodeConfig &config)
        {
            return std::make_unique<Node>(name,config,ros_node);
        }
    );
    std::string xml="/home/iamironman/ros2/src/cpp/xml/node.xml";
    auto tree=factory.createTreeFromFile(xml);
    BT::NodeStatus status=BT::NodeStatus::RUNNING;
    while(status==BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status=tree.tickOnce();
        rclcpp::spin_some(ros_node);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(ros_node->get_logger(),"tree run succesfully");
    }
    rclcpp::shutdown();
    return 0;
}