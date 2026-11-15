#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "sensor_msgs/msg/joint_state.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"

#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"


class DifferentialOdometry : public rclcpp::Node
{
public:

    DifferentialOdometry()
        : Node("differential_odometry"),
          x_(0.0),
          y_(0.0),
          theta_(0.0),
          previous_left_(0.0),
          previous_right_(0.0),
          first_reading_(true)
    {
        // Robot parameters
        wheel_radius_ = 0.05;     // meters
        wheel_base_   = 0.30;     // distance between wheels, meters

        // Publisher for odometry
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>(
            "/odom", 10);

        // TF broadcaster
        tf_broadcaster_ =
            std::make_unique<tf2_ros::TransformBroadcaster>(*this);

        // Subscribe to joint states
        joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states",
            10,
            std::bind(
                &DifferentialOdometry::jointCallback,
                this,
                std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(),
                    "Differential odometry node started");
    }


private:

    void jointCallback(
        const sensor_msgs::msg::JointState::SharedPtr msg)
    {
        double left_position = 0.0;
        double right_position = 0.0;

        bool left_found = false;
        bool right_found = false;

        // Find wheel joints
        for (size_t i = 0; i < msg->name.size(); i++)
        {
            if (i >= msg->position.size())
                continue;

            if (msg->name[i] == "left_wheel_joint")
            {
                left_position = msg->position[i];
                left_found = true;
            }

            if (msg->name[i] == "right_wheel_joint")
            {
                right_position = msg->position[i];
                right_found = true;
            }
        }

        if (!left_found || !right_found)
        {
            RCLCPP_WARN_THROTTLE(
                this->get_logger(),
                *this->get_clock(),
                2000,
                "Wheel joints not found in /joint_states");

            return;
        }

        // First measurement
        if (first_reading_)
        {
            previous_left_ = left_position;
            previous_right_ = right_position;
            first_reading_ = false;
            return;
        }

        // Change in wheel rotation
        double delta_left =
            left_position - previous_left_;

        double delta_right =
            right_position - previous_right_;

        previous_left_ = left_position;
        previous_right_ = right_position;

        // Distance travelled by each wheel
        double distance_left =
            wheel_radius_ * delta_left;

        double distance_right =
            wheel_radius_ * delta_right;

        // Robot movement
        double distance =
            (distance_right + distance_left) / 2.0;

        double delta_theta =
            (distance_right - distance_left) / wheel_base_;

        // Update position
        x_ += distance * std::cos(theta_ + delta_theta / 2.0);
        y_ += distance * std::sin(theta_ + delta_theta / 2.0);
        theta_ += delta_theta;

        // Normalize theta
        theta_ = std::atan2(
            std::sin(theta_),
            std::cos(theta_));

        publishOdometry();
    }


    void publishOdometry()
    {
        auto current_time = this->get_clock()->now();

        // -------------------------
        // TF: odom -> base_link
        // -------------------------

        geometry_msgs::msg::TransformStamped transform;

        transform.header.stamp = current_time;
        transform.header.frame_id = "odom";
        transform.child_frame_id = "base_footprint";

        transform.transform.translation.x = x_;
        transform.transform.translation.y = y_;
        transform.transform.translation.z = 0.0;

        tf2::Quaternion q;

        q.setRPY(
            0.0,
            0.0,
            theta_);

        transform.transform.rotation.x = q.x();
        transform.transform.rotation.y = q.y();
        transform.transform.rotation.z = q.z();
        transform.transform.rotation.w = q.w();

        tf_broadcaster_->sendTransform(transform);


        // -------------------------
        // Odometry message
        // -------------------------

        nav_msgs::msg::Odometry odom;

        odom.header.stamp = current_time;

        odom.header.frame_id = "odom";
        odom.child_frame_id = "base_footprint";

        // Position
        odom.pose.pose.position.x = x_;
        odom.pose.pose.position.y = y_;
        odom.pose.pose.position.z = 0.0;

        odom.pose.pose.orientation.x = q.x();
        odom.pose.pose.orientation.y = q.y();
        odom.pose.pose.orientation.z = q.z();
        odom.pose.pose.orientation.w = q.w();

        // Publish
        odom_pub_->publish(odom);
    }


    // ROS
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_sub_;

    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;

    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;


    // Robot parameters
    double wheel_radius_;
    double wheel_base_;


    // Robot pose
    double x_;
    double y_;
    double theta_;


    // Previous wheel positions
    double previous_left_;
    double previous_right_;

    bool first_reading_;
};


int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<DifferentialOdometry>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}