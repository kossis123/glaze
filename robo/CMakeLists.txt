
cmake_minimum_required(VERSION 3.8)
project(robo)

# Compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(interfaces REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)



function(add_ros_node node_name)

  
    add_executable(${node_name} ${ARGN})


    ament_target_dependencies(${node_name}
        rclcpp
        rclcpp_action
        interfaces
        behaviortree_cpp
        std_msgs
        sensor_msgs
        nav_msgs
        tf2
        tf2_ros
        geometry_msgs
    )

    install(TARGETS ${node_name}
        DESTINATION lib/${PROJECT_NAME}
    )

endfunction()



add_ros_node(hero
    src/action.cpp
)

add_ros_node(control
    src/controlelr.cpp
)

add_ros_node(teleop
    src/teleop.cpp
)

add_ros_node(odom
    src/odom_node.cpp
)

add_ros_node(hero2
    src/client.cpp
)

add_ros_node(hero3
    src/tree_1.cpp
    src/register.cpp
)

add_ros_node(bringup
    src/action_server/init_server.cpp
)

add_ros_node(client_init
    src/action_client/Init_client.cpp
    src/Tree_register/builder.cpp
    src/action_client/Init_up.cpp
)

add_ros_node(tf
    src/tf.cpp
)

add_ros_node(li
    src/li.cpp
)

add_ros_node(od
    src/odom.cpp
)

add_ros_node(noisy
    src/noisy_odom.cpp
)

add_ros_node(kalman
    src/kalman_filter.cpp
)

add_ros_node(bridge
    src/bridge_node.cpp
)

add_ros_node(er
    src/condom.cpp
)

add_ros_node(final
    src/action_server/lauda.cpp
)
add_ros_node(final2
    src/action_client/erlc.cpp
    src/Tree_register/re.cpp

)
install(
    DIRECTORY
        include
        launch
        urdf
        worlds
        config
    DESTINATION share/${PROJECT_NAME}
)




if(BUILD_TESTING)

    find_package(ament_lint_auto REQUIRED)

    set(ament_cmake_copyright_FOUND TRUE)
    set(ament_cmake_cpplint_FOUND TRUE)

    ament_lint_auto_find_test_dependencies()

endif()



ament_package()
