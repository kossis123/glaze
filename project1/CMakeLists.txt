cmake_minimum_required(VERSION 3.8)
project(project1)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)

add_executable(node src/check.cpp)
add_executable(control src/control_publisher.cpp)
add_executable(joy src/joy_check.cpp)

ament_target_dependencies(node
  rclcpp
  std_msgs
)
ament_target_dependencies(control
  rclcpp
  std_msgs
  trajectory_msgs
)
ament_target_dependencies(joy
  rclcpp
  std_msgs
  trajectory_msgs
  sensor_msgs
)

install(TARGETS
  node
  control
  joy
  DESTINATION lib/${PROJECT_NAME}
)

# Install launch files
install(
  DIRECTORY launch urdf config meshes worlds models
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()