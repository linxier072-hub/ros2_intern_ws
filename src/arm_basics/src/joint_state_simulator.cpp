#include <chrono>
#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

class JointStateSimulator : public rclcpp::Node
{
public:
  JointStateSimulator()
  : Node("joint_state_simulator"), elapsed_time_sec_(0.0)
  {
    publish_rate_hz_ =
      this->declare_parameter<double>("publish_rate_hz", 10.0);
    joint_name_ =
      this->declare_parameter<std::string>("joint_name", "joint1");
    amplitude_rad_ =
      this->declare_parameter<double>("amplitude_rad", 1.0);

    publisher_ =
      this->create_publisher<sensor_msgs::msg::JointState>(
        "/joint_states", 10);

    const auto timer_period =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(1.0 / publish_rate_hz_));

    timer_ = this->create_wall_timer(
      timer_period,
      [this]() { publish_joint_state(); });

    RCLCPP_INFO(
      this->get_logger(),
      "Simulating %s at %.1f Hz with amplitude %.2f rad",
      joint_name_.c_str(), publish_rate_hz_, amplitude_rad_);
  }

private:
  void publish_joint_state()
  {
    sensor_msgs::msg::JointState message;

    message.header.stamp = this->now();
    message.name = {joint_name_};
    message.position = {
      amplitude_rad_ * std::sin(elapsed_time_sec_)};

    publisher_->publish(message);

    elapsed_time_sec_ += 1.0 / publish_rate_hz_;
  }

  double publish_rate_hz_;
  std::string joint_name_;
  double amplitude_rad_;
  double elapsed_time_sec_;

  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JointStateSimulator>());
  rclcpp::shutdown();
  return 0;
}
