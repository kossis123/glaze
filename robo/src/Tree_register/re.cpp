#include<behaviortree_cpp/bt_factory.h>
#include<behaviortree_cpp/blackboard.h>
#include</home/iamironman/ros2/src/robo/include/robo/final.hpp>

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    auto node=std::make_shared<rclcpp::Node>("koshish");
    BT::BehaviorTreeFactory bt;
    bt.registerBuilder<robot>("run",
    [node](const std::string &string,const BT::NodeConfig &config){
        return std::make_unique<robot>(string,config,node); 
    });
    auto board=BT::Blackboard::create();
    board->set<float>("x_",1.2);
    board->set<float>("y_",2.3);
    auto tree=bt.createTreeFromFile("/home/iamironman/ros2/src/robo/tree/kera.xml",board);
    BT::NodeStatus status=BT::NodeStatus::RUNNING;
    while (status==BT::NodeStatus::RUNNING)
    {
        status=tree.tickOnce();
        rclcpp::spin_some(node);
    }
        if(status==BT::NodeStatus::SUCCESS)
    {
        RCLCPP_INFO(node->get_logger(),"tree succeded");
    }
    else
    {
        RCLCPP_INFO(node->get_logger(),"tree failed");
    }
 
    return 0;
}