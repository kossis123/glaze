#include<rclcpp/rclcpp.hpp>
#include<std_msgs/msg/string.hpp>

using namespace std::chrono_literals;
class node:public rclcpp::Node
{
    public:
    node():Node("koshish")
    {
        s_=this->create_publisher<std_msgs::msg::String>("topic",
            10
        ); 
        t_=this->create_wall_timer(
            10s,
            std::bind(&node::callback,this)
        );   
    }
    private:
    void callback()
    {
        std_msgs::msg::String msg;
        msg.data="koshi8sh";
        s_->publish(msg);

    }
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr s_;
    rclcpp::TimerBase::SharedPtr t_;
    
};
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}