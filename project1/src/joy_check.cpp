#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "trajectory_msgs/msg/joint_trajectory_point.hpp"


using namespace std;
using namespace std::chrono_literals;
class node:public rclcpp::Node
{
    public:

    node():Node("koshish")
    {
       
        s_=this->create_subscription<sensor_msgs::msg::Joy>("/joy",
        10,
        std::bind(&node::callback,this,std::placeholders::_1));
        
        p_=this->create_publisher<trajectory_msgs::msg::JointTrajectory>("/arm/joint_trajectory",
        10);

    }

    void callback(const sensor_msgs::msg::Joy::SharedPtr cmd)
    {
        
        trajectory_msgs::msg::JointTrajectory msg1;
        trajectory_msgs::msg::JointTrajectoryPoint msg2;
        msg1.joint_names={
            "Joint_1",
            "Joint_2",
            "Joint_3",
            "Joint_4",
            "Joint_5",
            "Joint_6",
            "Joint_Gripper"
        };

        if((cmd->axes[0]>0 && cmd->axes[0]<=1) || (cmd->axes[0]<0 && cmd->axes[0]>=-1))
        {
            
            targets[0]= targets[0] + section * cmd->axes[0];
            targets[0]= max(joint_limits[0],min(joint_limits[1],targets[0]));
            
        }
        if((cmd->axes[1]>0 && cmd->axes[1]<=1) || (cmd->axes[1]<0 && cmd->axes[1]>=-1))
        {
            targets[1]= targets[1] + section * cmd->axes[1];
            targets[1]= max(joint_limits[2],min(joint_limits[3],targets[1]));
        }
        
        if((cmd->axes[2]>0 && cmd->axes[2]<=1) || (cmd->axes[2]<0 && cmd->axes[2]>=-1))
        {
            targets[2]= targets[2] + section * cmd->axes[2];
            targets[2]= max(joint_limits[2],min(joint_limits[3],targets[2]));
        }

        if((cmd->axes[3]>0 && cmd->axes[3]<=1) || (cmd->axes[3]<0 && cmd->axes[3]>=-1))
        {
            targets[5]= targets[5] + section * cmd->axes[3];
            targets[5]= max(joint_limits[0],min(joint_limits[1],targets[5]));
        }

        if(cmd->buttons[0]==1 || cmd->buttons[2]==1)
        {
            targets[3]= targets[3] + section * cmd->buttons[0]-section * cmd->buttons[2];
            targets[3]= max(joint_limits[2],min(joint_limits[3],targets[3]));

        }
        if(cmd->buttons[3]==1 || cmd->buttons[1]==1)
        {
            
            targets[4]= targets[4] + section * cmd->buttons[3]-section * cmd->buttons[1];
            targets[4]= max(joint_limits[4],min(joint_limits[5],targets[4])); 

        }
        if(cmd->buttons[4]==1 || cmd->buttons[5]==1)
        {

            targets[6]= targets[6] + section * cmd->buttons[4]-section * cmd->buttons[5];
            targets[6]= max(joint_limits[6],min(joint_limits[7],targets[6])); 

        }
        
        msg2.time_from_start = rclcpp::Duration::from_seconds(0.001); 
        msg2.positions={targets[0], targets[1], targets[2], targets[3], targets[4], targets[5], targets[6]};
        msg1.points.push_back(msg2);
        p_->publish(msg1);
    }

    private:

    float joint_limits[8]={-3.14, 3.14, -1.57, 1.57, -1.46, 1.57, -0.5, 0.18};
    float section=0.095; 
    float targets[10]={0, 0, 0, 0, 0, 0, 0};
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr s_;
    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr p_;

};
int main(int argc, char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();

}