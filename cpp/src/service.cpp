#include</home/iamironman/ros2/src/cpp/include/cpp/hero.hpp>
using namespace std::chrono_literals;

    service::service(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_)
    :BT::StatefulActionNode(name,config),
    node(node_)
    {
        RCLCPP_INFO(node->get_logger(),"ready to send");
        c_=node->create_client<interfaces::srv::Node>("/server");
    
    }
     BT::PortsList service::providedPorts()
    {
        return{
        BT::InputPort<float>("target_x"),
        BT::InputPort<float>("target_y")
        };
    }
    BT::NodeStatus service::onStart() 
    {


        auto data=std::make_shared<interfaces::srv::Node::Request>();
        auto target_x=getInput<float>("target_x").value();
        auto target_y=getInput<float>("target_y").value();
        data->x=target_x;
        data->y=target_y;

        future=c_->async_send_request(data).share();

        return BT::NodeStatus::RUNNING;

    }
    BT::NodeStatus service::onRunning() 
    {
        if(future.wait_for(0s)!=std::future_status::ready)
        {
            return BT::NodeStatus::RUNNING;
        }
    
        auto response=future.get();
        if(!response->success)
        {
            return BT::NodeStatus::FAILURE;
        }
        else
        {
            return BT::NodeStatus::SUCCESS;
        }
        
    
    }
    void service::onHalted()
    {
        RCLCPP_INFO(node->get_logger(),"The prcoess is halted");
    }
    

