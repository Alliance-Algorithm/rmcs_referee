#include <cmath>

#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>
#include <rmcs_msgs/chassis_mode.hpp>

#include "app/ui/shape/shape.hpp"
#include "app/ui/widget/crosshair.hpp"

namespace rmcs_referee::app::ui {
using namespace std::chrono_literals;

class Infantry
    : public rmcs_executor::Component
    , public rclcpp::Node {
public:
    Infantry()
        : Node{get_component_name(), rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)}
        , crosshair_(Shape::Color::WHITE, x_center - 12, y_center - 37)
        , horizontal_center_guidelines_(
              {Shape::Color::WHITE, 2, x_center - 360, y_center, x_center - 110, y_center},
              {Shape::Color::WHITE, 2, x_center + 110, y_center, x_center + 360, y_center})
        , vertical_center_guidelines_(
              {Shape::Color::WHITE, 2, x_center, 800, x_center, y_center + 110},
              {Shape::Color::WHITE, 2, x_center, y_center - 110, x_center, 200})
        , yaw_indicator_(Shape::Color::WHITE, 20, 2, x_center, 860, 0)
        , yaw_indicator_guidelines_(
              {Shape::Color::WHITE, 2, x_center - 32, 830, x_center + 32, 830},
              {Shape::Color::WHITE, 2, x_center, 830, x_center, 820})
        , chassis_direction_indicator_(Shape::Color::PINK, 8, x_center, y_center, 0, 0, 84, 84) {
        yaw_indicator_.set_center_x(x_center);

        chassis_control_direction_indicator_.set_x(x_center);
        chassis_control_direction_indicator_.set_y(y_center);

        register_input("/chassis/mode", chassis_mode_);
        register_input("/chassis/angle", chassis_angle_);
        register_input("/chassis/control_angle", chassis_control_angle_);
    }

    void update() override {
        update_yaw_indicator();
        update_chassis_direction_indicator();
    }

private:
    void update_yaw_indicator() {
        double chassis_angle = *chassis_angle_;
        if (std::isnan(chassis_angle))
            chassis_angle = 0;
        yaw_indicator_.set_value(chassis_angle);
        yaw_indicator_.set_center_x(x_center);
    }

    void update_chassis_direction_indicator() {
        auto chassis_mode = *chassis_mode_;

        auto to_referee_angle = [](double angle) {
            return static_cast<int>(
                std::round((2 * std::numbers::pi - angle) / std::numbers::pi * 180));
        };
        chassis_direction_indicator_.set_color(
            chassis_mode == rmcs_msgs::ChassisMode::SPIN ? Shape::Color::GREEN
                                                         : Shape::Color::PINK);
        chassis_direction_indicator_.set_angle(to_referee_angle(*chassis_angle_), 30);

        bool chassis_control_direction_indicator_visible = false;
        if (!std::isnan(*chassis_control_angle_)) {
            if (chassis_mode == rmcs_msgs::ChassisMode::STEP_DOWN) {
                chassis_control_direction_indicator_visible = true;
                chassis_control_direction_indicator_.set_color(Shape::Color::CYAN);
                chassis_control_direction_indicator_.set_width(8);
                chassis_control_direction_indicator_.set_r(92);
                chassis_control_direction_indicator_.set_angle(
                    to_referee_angle(*chassis_control_angle_), 30);
            } else if (chassis_mode == rmcs_msgs::ChassisMode::LAUNCH_RAMP) {
                chassis_control_direction_indicator_visible = true;
                chassis_control_direction_indicator_.set_color(Shape::Color::CYAN);
                chassis_control_direction_indicator_.set_width(28);
                chassis_control_direction_indicator_.set_r(102);
                chassis_control_direction_indicator_.set_angle(
                    to_referee_angle(*chassis_control_angle_), 4);
            }
        }
        chassis_control_direction_indicator_.set_visible(
            chassis_control_direction_indicator_visible);
    }

    static constexpr uint16_t screen_width = 1920, screen_height = 1080;
    static constexpr uint16_t x_center = screen_width / 2, y_center = screen_height / 2;

    InputInterface<rmcs_msgs::ChassisMode> chassis_mode_;
    InputInterface<double> chassis_angle_, chassis_control_angle_;

    CrossHair crosshair_;

    Line horizontal_center_guidelines_[2];
    Line vertical_center_guidelines_[2];

    Float yaw_indicator_;
    Line yaw_indicator_guidelines_[2];

    Arc chassis_direction_indicator_, chassis_control_direction_indicator_;
};

} // namespace rmcs_referee::app::ui

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::app::ui::Infantry, rmcs_executor::Component)