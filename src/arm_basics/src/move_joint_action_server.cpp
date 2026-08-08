#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <thread>

#include "arm_basics/action/move_joint.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

class MoveJointActionServer : public rclcpp::Node
{
public:
  using MoveJoint = arm_basics::action::MoveJoint;
  using GoalHandleMoveJoint = rclcpp_action::ServerGoalHandle<MoveJoint>;

  MoveJointActionServer()
  : Node("move_joint_action_server")
  {
    using namespace std::placeholders;

    action_server_ = rclcpp_action::create_server<MoveJoint>(
      this,
      "/move_joint",
      std::bind(&MoveJointActionServer::handle_goal, this, _1, _2),
      std::bind(&MoveJointActionServer::handle_cancel, this, _1),
      std::bind(&MoveJointActionServer::handle_accepted, this, _1));

    RCLCPP_INFO(this->get_logger(), "MoveJoint Action Server ready");
  }

private:
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const MoveJoint::Goal> goal)
  {
    (void)uuid;

    if (goal->duration_sec <= 0.0) {
      RCLCPP_WARN(this->get_logger(), "Rejected goal: duration must be positive");
      return rclcpp_action::GoalResponse::REJECT;
    }

    RCLCPP_INFO(
      this->get_logger(), "Accepted goal: %.3f rad in %.2f s",
      goal->target_position_rad, goal->duration_sec);

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<GoalHandleMoveJoint> goal_handle)
  {
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "Accepted cancel request");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  void handle_accepted(
    const std::shared_ptr<GoalHandleMoveJoint> goal_handle)
  {
    std::thread([this, goal_handle]() { execute(goal_handle); }).detach();
  }

  void execute(const std::shared_ptr<GoalHandleMoveJoint> goal_handle)
  {
    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<MoveJoint::Feedback>();
    auto result = std::make_shared<MoveJoint::Result>();

    constexpr double feedback_hz = 10.0;
    const int steps = std::max(
      1, static_cast<int>(std::ceil(goal->duration_sec * feedback_hz)));
    const auto step_duration =
      std::chrono::duration<double>(goal->duration_sec / steps);

    for (int step = 1; step <= steps && rclcpp::ok(); ++step) {
      if (goal_handle->is_canceling()) {
        result->success = false;
        result->message = "Goal canceled";
        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "Goal canceled");
        return;
      }

      feedback->progress = static_cast<double>(step) / steps;
      feedback->current_position_rad =
        goal->target_position_rad * feedback->progress;
      goal_handle->publish_feedback(feedback);
      std::this_thread::sleep_for(step_duration);
    }

    if (!rclcpp::ok()) {
      return;
    }

    result->success = true;
    result->message = "Target position reached";
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Goal succeeded");
  }

  rclcpp_action::Server<MoveJoint>::SharedPtr action_server_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MoveJointActionServer>());
  rclcpp::shutdown();
  return 0;
}
