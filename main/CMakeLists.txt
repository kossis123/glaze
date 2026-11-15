cmake_minimum_required(VERSION 3.8)
project(main)
add_compile_definitions(ROBOTLIB_PLATFORM_LINUX)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)


add_executable(motor
  src/check.cpp


)

ament_target_dependencies(motor
  rclcpp
  geometry_msgs
)


add_executable(kin
  src/kinematics.cpp
)

ament_target_dependencies(kin
  rclcpp
  geometry_msgs
)

install(TARGETS
  motor
  kin
  DESTINATION lib/${PROJECT_NAME}
)


install(
  DIRECTORY include/
  DESTINATION include
)

ament_package()