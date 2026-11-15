from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution, Command
from launch_ros.substitutions import FindPackageShare
from launch.actions import ExecuteProcess, TimerAction

pkg = "cpp"


robot_urdf = PathJoinSubstitution([
    FindPackageShare(pkg),
    "urdf",
    "robot1.urdf"
])

robot_description = {
    "robot_description": Command(["cat ", robot_urdf])
}

box_urdf = PathJoinSubstitution([
    FindPackageShare(pkg),
    "urdf",
    "box.urdf"
])

box_description = {
    "box_description": Command(["cat ", box_urdf])
}


world_file = PathJoinSubstitution([
    FindPackageShare(pkg),
    "worlds",
    "model.sdf"
])


def generate_launch_description():

    ld = LaunchDescription()


    gz = ExecuteProcess(
        cmd=["ign", "gazebo", "-r", world_file],
        output="screen"
    )

    robot_rsp = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[robot_description],
        output="screen"
    )

    box_rsp = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[box_description],
        output="screen"
    )


    spawn_robot = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name", "hero",
            "-topic", "robot_description"
        ],
        output="screen"
    )



    joystick = Node(
        package="joy",
        executable="joy_node",
        output="screen"
    )

    bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            '/cmd_vel@geometry_msgs/msg/Twist@ignition.msgs.Twist',
            "/camera@sensor_msgs/msg/Image@gz.msgs.Image",
            "/camera/camera_info@sensor_msgs/msg/CameraInfo@gz.msgs.CameraInfo",
            "/camera/points@sensor_msgs/msg/PointCloud2@gz.msgs.PointCloudPacked",
            "/camera/depth_image@sensor_msgs/msg/Image@gz.msgs.Image",
            "/camera/rgb@sensor_msgs/msg/Image@gz.msgs.Image",
            '/scan@sensor_msgs/msg/LaserScan@ignition.msgs.LaserScan'
        ],
        output="screen"
    )


    controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["wheel"],
        output="screen"
    )


    ld.add_action(gz)

    ld.add_action(robot_rsp)
    ld.add_action(box_rsp)

    ld.add_action(TimerAction(period=3.0, actions=[spawn_robot]))

    ld.add_action(joystick)
    ld.add_action(bridge)
    ld.add_action(controller)

    return ld