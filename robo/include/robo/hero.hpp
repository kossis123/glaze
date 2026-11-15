#include "rclcpp/rclcpp.hpp"
#include <tf2_ros/static_transform_broadcaster.hpp>
#include "tf2/LinearMath/Quanternion.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

class node:public rclcpp::Node
{
    public:
    node();
    private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> b_;
    tf2::Quanternion q;
    geometry_msgs::msg::TransformStamped msg;
    
};