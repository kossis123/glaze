#include<rclcpp/rclcpp.hpp>
#include<behaviortree_cpp/action_node.h>
class hero:public BT::StatefulActionNode
{
    public:
        hero(const std::string &name,const BT::NodeConfig &config):BT::StatefulActionNode(name,config)
    {

    }
    static BT::PortsList providedPorts() 
    {
        return{
        BT::InputPort<int>("target_value"),
        BT::OutputPort<int>("count")
        };
    }
    BT::NodeStatus onStart() override
    {
        RCLCPP_INFO(rclcpp::get_logger("hero"),"Tree Started");
        if(!getInput<int>("target_value",target))
        {
            throw BT::RuntimeError("no value");
        }
        else{
            RCLCPP_INFO(rclcpp::get_logger("hero"),"Node Started");
            return BT::NodeStatus::RUNNING;
        }        
    }
    BT::NodeStatus onRunning() override{
        count_++;
        setOutput("count",count_);
        if(count_>5)
        {
            return BT::NodeStatus::SUCCESS;
        }
        else 
        {
            return BT::NodeStatus::RUNNING;
        }
    }
    void onHalted() override
    {
        RCLCPP_INFO(rclcpp::get_logger("hero"),"Halted");
    }


    
    private:
    int count_=0;
    int target;
    };