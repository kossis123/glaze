cmake_minimum_required(VERSION 3.8)
project(interfaces)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/Node.srv"
  "action/Node.action"
  "action/Check.action"
  "action/Init.action"
  "action/Er.action"

)

ament_package()