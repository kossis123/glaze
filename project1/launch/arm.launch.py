from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution, Command
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterValue
from launch.actions import ExecuteProcess, TimerAction

pkg = "project1"


def generate_launch_description():
    
    box_model = PathJoinSubstitution([
    FindPackageShare("project1"),
    "models",
    "cylinder",
    "model.sdf"
])
    world_file = PathJoinSubstitution([
        FindPackageShare(pkg),
        "worlds",
        "con.sdf"
    ])

    robot_desc = PathJoinSubstitution([
        FindPackageShare(pkg),
        "urdf",
        "arm_description.urdf"
    ])

    box_desc = PathJoinSubstitution([
        FindPackageShare(pkg),
        "urdf",
        "cube.urdf"
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
        parameters=[robot_description],
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


    
#     spawn_box = Node(
#     package="ros_gz_sim",
#     executable="create",
#     arguments=[
#         "-name", "box",
#         "-file", box_model,
#         "-x", "0.3",
#         "-y", "0.1",
#         "-z", "0.5"
#     ],
#     output="screen"
# )

    jsb_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager", "/controller_manager",
            "--controller-type", "joint_state_broadcaster/JointStateBroadcaster"
        ],
        output="screen"
    )


    arm_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["arm"],
        output="screen"
    )

   
    bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/camera@sensor_msgs/msg/Image@gz.msgs.Image",
            "/camera/camera_info@sensor_msgs/msg/CameraInfo@gz.msgs.CameraInfo",
            "/camera/points@sensor_msgs/msg/PointCloud2@gz.msgs.PointCloudPacked",
            "/camera/depth_image@sensor_msgs/msg/Image@gz.msgs.Image",
            "/camera/rgb@sensor_msgs/msg/Image@gz.msgs.Image"
        ],
        output="screen",
    )

    return LaunchDescription([
        gz,

        robot_state_pub,

        jsb_controller,

        TimerAction(
            period=4.0,
            actions=[spawn_robot]
        ),

        # TimerAction(
        #     period=5.0,
        #     actions=[spawn_box]
        # ),

        TimerAction(
            period=7.0,
            actions=[arm_controller]
        ),

        bridge
    ])