#include</home/iamironman/ros2/src/cpp/include/cpp/hero2.hpp>
using namespace std::chrono_literals;

    client::client(const std::string &name,const BT::NodeConfig &config,rclcpp::Node::SharedPtr node_)
    :BT::StatefulActionNode(name,config),
    node(node_)
    {
        RCLCPP_INFO(node->get_logger(),"ready to send");
        c_=node->create_client<interfaces::srv::Node>("/server");
    
    }
     BT::PortsList client::providedPorts()
    {
        return{
        BT::InputPort<float>("target_fx"),
        BT::InputPort<float>("target_fy")
        };
    }
    BT::NodeStatus client::onStart() 
    {

        if(!c_->wait_for_service(0s))
        {
            RCLCPP_WARN(node->get_logger()," Waiting For Connection ");
            return BT::NodeStatus::FAILURE;

        }
        auto data=std::make_shared<interfaces::srv::Node::Request>();
        auto target_x=getInput<float>("target_fx").value();
        auto target_y=getInput<float>("target_fy").value();
        data->x=target_x;
        data->y=target_y;

        future=c_->async_send_request(data).share();

        return BT::NodeStatus::RUNNING;

    }
    BT::NodeStatus client::onRunning() 
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
    void client::onHalted()
    {
        RCLCPP_INFO(node->get_logger(),"The prcoess is halted");
    }
    

