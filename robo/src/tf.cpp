#include</home/iamironman/ros2/src/robo/include/robo/tf.hpp>

tf::tf()
:Node("koshish"),
increment(1),
last(0)
{

    broadcaster_=std::make_shared<tf2_ros::StaticTransformBroadcaster>(*this);
    dynamic_=std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    t_=this->create_wall_timer(0.1s,
    std::bind(&tf::callback,this)
);
    transform.header.stamp=get_clock()->now();
    transform.header.frame_id="base_link";
    transform.child_frame_id="child_link";
    transform.transform.translation.x=0;
    transform.transform.translation.y=1;
    transform.transform.translation.z=0;    
    transform.transform.rotation.x=0;
    transform.transform.rotation.y=0;
    transform.transform.rotation.z=0;
    transform.transform.rotation.w=1;
    broadcaster_->sendTransform(transform);
    RCLCPP_INFO(this->get_logger(),"Node started");
    last1.setRPY(0,0,0);
    change.setRPY(0,0,0.5);

}
void tf::callback()
{
    d_transform.header.stamp=get_clock()->now();
    d_transform.header.frame_id="odom";
    d_transform.child_frame_id="base_link";
    d_transform.transform.translation.x=last+increment;
    d_transform.transform.translation.y=0.0;
    d_transform.transform.translation.z=0.0;
    tf2::Quaternion q;
    q=last1*change;
    last1=q;
    d_transform.transform.rotation.x=q.x();
    d_transform.transform.rotation.y=q.y();
    d_transform.transform.rotation.z=q.z();
    d_transform.transform.rotation.w=q.w();
    // last=d_transform.transform.translation.x;
    dynamic_->sendTransform(d_transform);


}
int main(int argc,char *argv[])
{

    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<tf>());
    rclcpp::shutdown();

}
