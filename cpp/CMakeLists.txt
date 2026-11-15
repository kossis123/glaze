cmake_minimum_required(VERSION 3.8)
project(cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(interfaces REQUIRED)
find_package(turtlesim REQUIRED)
find_package(rclcpp_action REQUIRED)

add_executable(node1 src/publisher.cpp)
add_executable(node2 src/joystick.cpp)
add_executable(node3 src/bt.cpp)
add_executable(node4 src/re.cpp)
add_executable(node5 src/node.cpp)
add_executable(node6 src/subscriber.cpp)
add_executable(node7 src/dual.cpp)
add_executable(main 
  src/main.cpp
  src/service.cpp
  src/service2.cpp  
)
add_executable(bot 

src/action_client/bot_register.cpp
src/action_client/turtle_start.cpp
src/action_client/turtle_end.cpp)
add_executable(server src/server/bot_server.cpp)
install(DIRECTORY launch  urdf worlds
  DESTINATION share/${PROJECT_NAME}
)
ament_target_dependencies(node1
nav_msgs
rclcpp
std_msgs
sensor_msgs

)
ament_target_dependencies(node2
std_msgs
sensor_msgs
rclcpp)
ament_target_dependencies(node3 
std_msgs
rclcpp
behaviortree_cpp
)
ament_target_dependencies(node4 
std_msgs
rclcpp
behaviortree_cpp
)

ament_target_dependencies(node6 
std_msgs
rclcpp
behaviortree_cpp
)
ament_target_dependencies(node5 
std_msgs
rclcpp
behaviortree_cpp
)
ament_target_dependencies(node7
std_msgs
rclcpp
behaviortree_cpp
)
ament_target_dependencies(main
std_msgs
rclcpp
behaviortree_cpp
interfaces
)

ament_target_dependencies(bot
std_msgs
rclcpp
behaviortree_cpp
interfaces
turtlesim
geometry_msgs
rclcpp_action
)
ament_target_dependencies(server
std_msgs
rclcpp
behaviortree_cpp
interfaces
turtlesim
geometry_msgs
rclcpp_action
)


install(TARGETS 
node1
node2
node3
node4
node5
node6
node7
main
server

bot
  DESTINATION lib/${PROJECT_NAME}
)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
