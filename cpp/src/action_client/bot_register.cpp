#include</home/iamironman/ros2/src/cpp/include/cpp/turtle_start.hpp>
#include</home/iamironman/ros2/src/cpp/include/cpp/turtle_end.hpp>

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    auto hero=std::make_shared<rclcpp::Node>("Koshish");
    BT::BehaviorTreeFactory factory;
    factory.registerBuilder<move_start>("move_start",
    [hero](const std::string &name,const BT::NodeConfig &config)
    {
        return std::make_unique<move_start>(name,config,hero);
    }
    );
    factory.registerBuilder<move_end>("move_end",
    [hero](const std::string &name,const BT::NodeConfig &config)
    {
        return std::make_unique<move_end>(name,config,hero);
    }
    );
    const std::string xml="/home/iamironman/ros2/src/cpp/xml/turtle.xml";
    auto tree=factory.createTreeFromFile(xml);
    BT::NodeStatus status=BT::NodeStatus::RUNNING;
    while(status==BT::NodeStatus::RUNNING && rclcpp::ok())
    {
        status=tree.tickOnce();
        rclcpp::spin_some(hero);
    }
    if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(hero->get_logger(),"tree succeded");
    }
    else
    {
        RCLCPP_INFO(hero->get_logger(),"tree failed");
    }
 
    return 0;
}