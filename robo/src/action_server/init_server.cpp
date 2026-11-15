#include<rclcpp/rclcpp.hpp>
#include<rclcpp_action/rclcpp_action.hpp>
#include<interfaces/action/init.hpp>
#include<thread>
#include<std_msgs/msg/float64_multi_array.hpp>
#include<sensor_msgs/msg/joint_state.hpp>


class node:public rclcpp::Node
{
    public:
    node():Node("INIT_NODE")
    {

        s_=rclcpp_action::create_server<interfaces::action::Init>(
            this,
            "/server",
            std::bind(&node::response_handle,this,std::placeholders::_1,std::placeholders::_2),
            std::bind(&node::cancel_handle,this,std::placeholders::_1),
            std::bind(&node::result_handle,this,std::placeholders::_1)
        );
        
        pub_=this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/slider/commands",
            10
        );

        sub_=this->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states",
            10,
            std::bind(&node::callback,this,std::placeholders::_1)
        );

        RCLCPP_INFO(this->get_logger(),"The server has been started");

    }
    private:

    rclcpp_action::GoalResponse response_handle(const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const interfaces::action::Init::Goal> goal)
    {
        
        (void)uuid;
        (void)goal;
        RCLCPP_INFO(this->get_logger(),"The command has been recieved");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;

    }

    rclcpp_action::CancelResponse cancel_handle(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Init>> goal_handle)
    {
        
        (void)goal_handle;
        RCLCPP_WARN(this->get_logger(),"The goal is canceled");
        return rclcpp_action::CancelResponse::ACCEPT;

    }

    void result_handle(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Init>> goal_handle)
    {

        std::thread(&node::execute,this,goal_handle).detach();

    }

    void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<interfaces::action::Init>> goal_handle)
    {
        
        auto goal=goal_handle->get_goal();
        auto result=std::make_shared<interfaces::action::Init::Result>();
        auto cmd=std_msgs::msg::Float64MultiArray();
        cmd.data={goal->value[0]};
        pub_->publish(cmd);
        result->success=true;
        goal_handle->succeed(result);   
        RCLCPP_INFO(this->get_logger(),"The Goal is sent");
        
    }

    void callback(sensor_msgs::msg::JointState::SharedPtr cm)
    {
        pose=cm->position[3];
    }
    rclcpp_action::Server<interfaces::action::Init>::SharedPtr s_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr sub_;
    float pose;
    
};

int main(int argc,char *argv[])
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<node>());
    rclcpp::shutdown();
}