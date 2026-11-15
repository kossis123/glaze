#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>

class node:public rclcpp::Node
{
    public:
    node():Node("koshish"),
    variance_(1000),
    mean_(0),
    motion(0),
    motion_variance(4.0),
    measured_variance_(0.5),
    first_(true),
    last_angular(0)
    {
        
        odom_=this->create_subscription<nav_msgs::msg::Odometry>("/odom_noisy",
        10,
        std::bind(&node::odom_callback,this,std::placeholders::_1));
        
        imu_=this->create_subscription<sensor_msgs::msg::Imu>("/imu",
        10,
        std::bind(&node::imu_callback,this,std::placeholders::_1));

        p_=this->create_publisher<nav_msgs::msg::Odometry>("/filtered",
        10);
        
    }
    private:

    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr odom)
    {

        if(first_)
        {
            mean_=odom->twist.twist.angular.z;
            last_angular=odom->twist.twist.angular.z;
            first_=false;
            return;
        }
        
        motion = odom->twist.twist.angular.z - last_angular;
        last_angular=odom->twist.twist.angular.z;

        state_prediction();
        measurement_update();
        pub.twist.twist.angular.z=mean_;
        p_->publish(pub);
        
    }
    
    void imu_callback(const sensor_msgs::msg::Imu::SharedPtr imu)
    {
        angular=imu->angular_velocity.z;
    }

    void measurement_update()
    {

        mean_ = (mean_ * measured_variance_ + angular * variance_)/(measured_variance_ + variance_);
        variance_=(measured_variance_ * variance_) / (measured_variance_ + variance_);

    }

    void state_prediction()
    {

        mean_ = mean_ + motion;
        variance_=variance_ + motion_variance;
    
    }

    double variance_;
    double mean_;
        double motion;
    double motion_variance;
    double measured_variance_;
    bool first_;
    float last_angular;

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr p_;
    nav_msgs::msg::Odometry pub;
    float angular;

};

int main(int argc, char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
    return 0;
}