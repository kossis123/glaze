#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <cmath>


class odom:public rclcpp::Node
{
    public:
    odom():Node("hero")
    {
        
        s_=this->create_subscription<sensor_msgs::msg::JointState>("/joint_states",
        10,
        std::bind(&odom::callback,this,std::placeholders::_1)
        );
        p_=this->create_publisher<nav_msgs::msg::Odometry>("/odom",
        10);
        last_time=this->get_clock()->now();
    }

    private:
    void callback(const sensor_msgs::msg::JointState::SharedPtr states)
    {

        rclcpp::Time current_time=this->get_clock()->now();
        left_wheel=states->position[0] ;
        right_wheel=states->position[1] ;

        dt = (current_time - last_time).seconds(); 
        last_time=current_time;

        dell_theta1 = left_wheel - previous_left;
        dell_theta2 = right_wheel-previous_right;

        previous_left = left_wheel;
        previous_right = right_wheel;

        left_s=radius * dell_theta1;
        right_s=radius * dell_theta2;

        average = (left_s + right_s) / 2;
        dell_yaw = (right_s - left_s) / seperation;
        del_x = average * cos(yaw);
        del_y = average * sin(yaw);

        velocity_l = left_s / dt;
        velocity_r = right_s / dt;

        linear_x=(velocity_l + velocity_r) / 2;
        angular_z=(velocity_r - velocity_l) / seperation;

        x+=del_x;
        y+=del_y;
        yaw+=dell_yaw;

        auto odom=nav_msgs::msg::Odometry();
        odom.header.frame_id = "odom_link";
        odom.child_frame_id = "base_footprint";
        odom.header.stamp = this->get_clock()->now();
        odom.pose.pose.position.x = x;
        odom.pose.pose.position.y = y;
        odom.twist.twist.linear.x = linear_x;
        odom.twist.twist.angular.z = angular_z;

        q_.setRPY(0,0,yaw);

        odom.pose.pose.orientation.x=q_.x();
        odom.pose.pose.orientation.y=q_.y();
        odom.pose.pose.orientation.z=q_.z();
        odom.pose.pose.orientation.w=q_.w();

        p_->publish(odom);
    }

    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr s_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr p_;

    float left_wheel;
    float right_wheel;
    float radius=0.08;
    float seperation=0.36;
    float left_s;
    float right_s;
    float average;
    float dell_yaw;
    float del_x;
    float yaw=0;
    float del_y;
    float x=0;
    float y=0;
    float previous_left=0;
    float previous_right=0;
    float dell_theta1;
    float dell_theta2;
    float dt;
    float velocity_l;
    float velocity_r;
    float linear_x;
    float angular_z;
    rclcpp::Time last_time;
    tf2::Quaternion q_;

};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<odom>());
    rclcpp::shutdown();
}