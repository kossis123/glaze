from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution, Command
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import ExecuteProcess, TimerAction

pkg = "robo"


def generate_launch_description():

    world_file = PathJoinSubstitution([
        FindPackageShare(pkg),
        "worlds",
        "hero.sdf"
    ])

    robot_desc = PathJoinSubstitution([
        FindPackageShare(pkg),
        "urdf",
        "robot.urdf"
    ])

    robot_description = {
        "robot_description": ParameterValue(
            Command(["cat ", robot_desc]),
            value_type=str
        )
    }

    gz = ExecuteProcess(
        cmd=["ign", "gazebo", "-r", world_file],
        output="screen"
    )

    robot_state_pub = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[
            robot_description,
            {"use_sim_time": True}
        ],
        output="screen"
    )

    sensor_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        parameters=[{"use_sim_time": True}],
        arguments=[
            '/clock@rosgraph_msgs/msg/Clock[ignition.msgs.Clock',
            '/scan@sensor_msgs/msg/LaserScan[ignition.msgs.LaserScan',
            '/imu@sensor_msgs/msg/Imu[ignition.msgs.IMU',
        ],
        output="screen"
    )

    spawn_robot = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name", "hero",
            "-topic", "robot_description",
            "-x", "-2",
            "-y", "-2",
            "-z", "3"
        ],
        output="screen"
    )

    jsb_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager"
        ],
        parameters=[{"use_sim_time": True}],
        output="screen"
    )

    arm_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["wheel"],
        parameters=[{"use_sim_time": True}],
        output="screen"
    )

    odom_node = Node(
        package="robo",
        executable="odom",
        parameters=[{"use_sim_time": True}],
        output="screen"
    )

    return LaunchDescription([
        gz,
        robot_state_pub,
        sensor_bridge,
        TimerAction(
            period=4.0,
            actions=[spawn_robot, jsb_controller]
        ),
        TimerAction(
            period=6.0,
            actions=[arm_controller, odom_node]
        ),
    ])