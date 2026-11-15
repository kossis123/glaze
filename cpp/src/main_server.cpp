#include<rclcpp/rclcpp.hpp>
#include<interfaces/srv/node.hpp>
#include<geometry_msgs/msg/twist.hpp>
#include<turtlesim/msg/pose.hpp>

class node:public rclcpp::Node
{
public:
    node():Node("koshish"){
    s_=this->create_service<interfaces::srv::Node>("/server",
    std::bind(&node::callback_service,this,std::placeholders::_1,std::placeholders::_2)
    );
    pub_=this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel",
        10);
    sub_=this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose",
        10,
        std::bind(&node::callback_client,this,std::placeholders::_1)
    );
}

private:
void callback_service(interfaces::srv::Node::Request::SharedPtr req,interfaces::srv::Node::Response::SharedPtr res)
{   
        
        res->success=true;
        RCLCPP_INFO(this->get_logger(),"%f%f",req->x,req->y);
        goal_x=req->x;
        goal_y=req->y;
        has_goal=true;
        
}
void callback_client(turtlesim::msg::Pose::SharedPtr msg)
{
   
        if(has_goal){
        auto message=geometry_msgs::msg::Twist();
        double dx=goal_x - msg->x;
        double dy=goal_y-msg->y;
        double desired_angle=atan2(dy,dx);
        double error=desired_angle -msg->theta;
        double dist1=sqrt(pow(dx,2)+pow(dy,2));
        RCLCPP_INFO(this->get_logger(),"%f \t%f",msg->x,msg->y);
         
        if(dist1>0.1){
        message.angular.z=5*error; 
        message.linear.x=2.0;
        }
        else{

            message.linear.x=0.0;
           
        }
        pub_->publish(message);
    }
}
    rclcpp::Service<interfaces::srv::Node>::SharedPtr s_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;   
     
    double goal_x=0.0;
    double goal_y=0.0;
    bool has_goal=false;
};
int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}