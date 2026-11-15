#include</home/iamironman/ros2/src/cpp/include/cpp/hero.hpp>
#include</home/iamironman/ros2/src/cpp/include/cpp/hero2.hpp>
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    auto hero=std::make_shared<rclcpp::Node>("hero_node");
    BT::BehaviorTreeFactory factory;
    factory.registerBuilder<service>("StartingNode",
    [hero](const std::string &name,const BT::NodeConfig &config)
    {
        return std::make_unique<service>(name,config,hero);
    });

    factory.registerBuilder<client>("FinishingNode",
    [hero](const std::string &name,const BT::NodeConfig &config)
    {
        return std::make_unique<client>(name,config,hero);
    });

    const std::string xml="/home/iamironman/ros2/src/cpp/xml/server.xml";
    auto tree=factory.createTreeFromFile(xml);
    BT::NodeStatus status=BT::NodeStatus::RUNNING;
    while(status==BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status=tree.tickOnce();
        rclcpp::spin_some(hero);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }
    if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(hero->get_logger(),"Tree Run sucessfully");
    }
    else if(status==BT::NodeStatus::FAILURE)
    {
        RCLCPP_INFO(hero->get_logger(),"Tree Failed");
    
    }
    rclcpp::shutdown();
}