#include "/home/iamironman/ros2/src/robo/include/robo/init.hpp"
#include "/home/iamironman/ros2/src/robo/include/robo/init2.hpp"

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    auto hero=std::make_shared<rclcpp::Node>("koshish");
    BT::BehaviorTreeFactory bt;
    bt.registerBuilder<start>("Init_down",
        [hero](const std::string &name,const BT::NodeConfig &config){
            return std::make_unique<start>(name ,config ,hero);
        }
    );
    bt.registerBuilder<end>("Init_up",
        [hero](const std::string &name,const BT::NodeConfig &config){
            return std::make_unique<end>(name ,config ,hero);
        }
    );
    auto tree=bt.createTreeFromFile("/home/iamironman/ros2/src/robo/tree/kera.xml");
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