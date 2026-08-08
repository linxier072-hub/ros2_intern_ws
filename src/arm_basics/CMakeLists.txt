cmake_minimum_required(VERSION 3.8)
project(arm_basics)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "action/MoveJoint.action"
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(status_publisher src/status_publisher.cpp)
ament_target_dependencies(status_publisher rclcpp std_msgs)

add_executable(status_subscriber src/status_subscriber.cpp)
ament_target_dependencies(status_subscriber rclcpp std_msgs)

add_executable(
  joint_state_simulator
  src/joint_state_simulator.cpp
)
ament_target_dependencies(
  joint_state_simulator
  rclcpp
  sensor_msgs
  std_srvs
)

add_executable(
  move_joint_action_server
  src/move_joint_action_server.cpp
)
ament_target_dependencies(
  move_joint_action_server
  rclcpp
  rclcpp_action
)
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME}
  "rosidl_typesupport_cpp"
)
target_link_libraries(
  move_joint_action_server
  "${cpp_typesupport_target}"
)
install(TARGETS
  status_publisher
  status_subscriber
  joint_state_simulator
  move_joint_action_server
  DESTINATION lib/${PROJECT_NAME}
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
