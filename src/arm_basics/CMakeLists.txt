cmake_minimum_required(VERSION 3.8)
project(arm_basics)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # Skip copyright and cpplint checks while learning.
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(status_publisher src/status_publisher.cpp)
ament_target_dependencies(status_publisher rclcpp std_msgs)

add_executable(status_subscriber src/status_subscriber.cpp)
ament_target_dependencies(status_subscriber rclcpp std_msgs)

install(TARGETS
  status_publisher
  status_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
