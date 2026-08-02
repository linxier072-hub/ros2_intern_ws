#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class ArmStatusPublisher : public rclcpp::Node
{
public:
  ArmStatusPublisher()
  : Node("arm_status_publisher"), count_(0)
  {
    publisher_ = create_publisher<std_msgs::msg::String>("arm_status", 10);
    timer_ = create_wall_timer(
      500ms, std::bind(&ArmStatusPublisher::publish_status, this));
  }

private:
  void publish_status()
  {
    auto message = std_msgs::msg::String();
    message.data = "SCUT arm online #" + std::to_string(count_++);
    RCLCPP_INFO(get_logger(), "Publishing: '%s'", message.data.c_str());
    publisher_->publish(message);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ArmStatusPublisher>());
  rclcpp::shutdown();
  return 0;
}
