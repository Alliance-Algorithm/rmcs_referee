#include <eigen3/Eigen/Eigen>

#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>

#include "rmcs_executor/component.hpp"

namespace rmcs_referee::command::interaction {

class Communicate
    : public rmcs_executor::Component
    , public rclcpp::Node {

public:
    Communicate()
        : rclcpp::Node(get_component_name()) {}

    void update() override {}
};

} // namespace rmcs_referee::command::interaction

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::command::interaction::Communicate, rmcs_executor::Component)