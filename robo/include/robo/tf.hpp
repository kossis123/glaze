#include <rclcpp/rclcpp.hpp>
#include <tf2_ros/static_transform_broadcaster.hpp>
#include <tf2_ros/transform_broadcaster.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <memory>
using namespace std::chrono_literals;
class tf:public rclcpp::Node
{
    public:
    tf();

    private:
    void callback();
    int increment;
    int last;
    std::shared_ptr<tf2_ros::TransformBroadcaster> dynamic_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
    geometry_msgs::msg::TransformStamped transform;
    geometry_msgs::msg::TransformStamped d_transform;
    tf2::Quaternion last1;
    tf2::Quaternion change;
    rclcpp::TimerBase::SharedPtr t_;

};