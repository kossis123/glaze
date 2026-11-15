#include<rclcpp/rclcpp.hpp>
#include<std_msgs/msg/string.hpp>
class node:public rclcpp::Node
{
    public:
    node():Node("koshish")
    {
        s_=this->create_subscription<std_msgs::msg::String>("/msg",
            10,
            std::bind(&node::callback,this,std::placeholders::_1)
        );
    }
    private:
    void callback(std_msgs::msg::String::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(),"%s message recieved",msg->data.c_str());
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr s_;
};
int main(int argc,char *argv[])
{
    
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();

}