#include<rclcpp/rclcpp.hpp>
#include<sensor_msgs/msg/joy.hpp>
#include<std_msgs/msg/float64_multi_array.hpp>
class node:public rclcpp::Node
{
    public:
    node():Node("koshish"){
    s_=this->create_subscription<sensor_msgs::msg::Joy>("/joy",
    10,
    std::bind(&node::callback,this,std::placeholders::_1)
);
    p_=this->create_publisher<std_msgs::msg::Float64MultiArray>("/wheel/commands",
    10);
    }

    private:
    void callback(sensor_msgs::msg::Joy::SharedPtr msg)
    {
        std_msgs::msg::Float64MultiArray icon;
        auto cmd=msg->axes;
        float up_and_down_key=cmd[1];
        float left_and_right_key=cmd[0];
        if(up_and_down_key==1)
        {
            icon.data={15,15,15,15};
        }
        else if(up_and_down_key==-1)
        {
            icon.data={-15,-15,-15,-15};
        }
        else if(left_and_right_key==1)
        {
            icon.data={0,15,0,15};
        }
        else if( left_and_right_key==-1)
        {
            icon.data={15,0,15,0};
        }
        else{
            icon.data={0,0,0,0};
        }
        p_->publish(icon);
    }
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr s_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr p_;
};
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}