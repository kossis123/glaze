#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <vector>

class node:public rclcpp::Node
{
    public:
    node():Node("KOSHISH")
    {

        s_=this->create_subscription<sensor_msgs::msg::LaserScan>("/scan",
        10,
        std::bind(&node::callback,this,std::placeholders::_1));

    }

    private:
    void callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        
        store=msg->angle_min;
        increment=msg->angle_increment;
        position_callback(store,increment,msg->ranges);
        

    }

    void position_callback(float store,float increment,const std::vector<float>& hero)
    {
        
        for(size_t i=0 ;i<hero.size() ;i++ )
        {
            if(store>-0.52 && store<0.52)
            {
                arrays.push_back(hero[i]);
            }
            store=store + increment;
        }
        for(size_t i=0;i<arrays.size();i++)
        {
            RCLCPP_INFO(this->get_logger(),"%f",arrays[i]);
        }
        
        rclcpp::shutdown();
    }

    std::vector<float> arrays;
    float store;
    float increment;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr s_;

};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}