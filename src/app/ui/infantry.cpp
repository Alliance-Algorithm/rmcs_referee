#include <cmath>

#include <cstdint>
#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>
#include <rmcs_msgs/chassis_mode.hpp>

#include "app/ui/shape/shape.hpp"
#include "app/ui/widget/crosshair.hpp"
#include "app/ui/widget/status_ring.hpp"

namespace rmcs_referee::app::ui {
using namespace std::chrono_literals;

class Infantry
    : public rmcs_executor::Component
    , public rclcpp::Node {
public:
    Infantry()
        : Node{get_component_name(), rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)}
        , crosshair_(Shape::Color::WHITE, x_center - 12, y_center - 37)
        , status_ring_()
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
        , chassis_direction_indicator_(Shape::Color::PINK, 8, x_center, y_center, 0, 0, 84, 84)
        , chassis_voltage_indicator_(Shape::Color::WHITE, 20, 2, x_center + 10, 820, 0)
        , chassis_power_indicator_(Shape::Color::WHITE, 20, 2, x_center + 10, 790, 0)
        , chassis_control_power_limit_indicator_(Shape::Color::WHITE, 20, 2, x_center + 10, 760, 0)
        , supercap_control_power_limit_indicator_(Shape::Color::WHITE, 20, 2, x_center + 10, 730, 0)
        , chassis_wheel_velocity_indicators_(
              {Shape::Color::WHITE, 20, 2, x_center + 10, 700, 0},
              {Shape::Color::GREEN, 20, 2, x_center + 10, 670, 0},
              {Shape::Color::WHITE, 20, 2, x_center + 170, 670, 0},
              {Shape::Color::GREEN, 20, 2, x_center + 170, 700, 0}) {
        yaw_indicator_.set_center_x(x_center);

        chassis_control_direction_indicator_.set_x(x_center);
        chassis_control_direction_indicator_.set_y(y_center);

        register_input("/chassis/control_mode", chassis_mode_);

        register_input("/chassis/angle", chassis_angle_);
        register_input("/chassis/control_angle", chassis_control_angle_);

        register_input("/chassis/supercap/voltage", supercap_voltage_);
        register_input("/chassis/supercap/enabled", supercap_enabled_);

        register_input("/chassis/voltage", chassis_voltage_);
        register_input("/chassis/power", chassis_power_);
        register_input("/chassis/control_power_limit", chassis_control_power_limit_);
        register_input("/chassis/supercap/control_power_limit", supercap_control_power_limit_);

        register_input("/chassis/left_front_wheel/velocity", left_front_velocity_);
        register_input("/chassis/left_back_wheel/velocity", left_back_velocity_);
        register_input("/chassis/right_back_wheel/velocity", right_back_velocity_);
        register_input("/chassis/right_front_wheel/velocity", right_front_velocity_);

        register_input("/referee/shooter/bullet_allowance", robot_bullet_allowance_);

        status_ring_.set_limits(500, 500, 400, 400);
    }

    void update() override {
        update_yaw_indicator();
        update_chassis_direction_indicator();

        chassis_voltage_indicator_.set_value(*chassis_voltage_);
        chassis_power_indicator_.set_value(*chassis_power_);
        chassis_control_power_limit_indicator_.set_value(*chassis_control_power_limit_);
        supercap_control_power_limit_indicator_.set_value(*supercap_control_power_limit_);

        // chassis_wheel_velocity_indicators_[0].set_value(*left_front_velocity_);
        // chassis_wheel_velocity_indicators_[1].set_value(*left_back_velocity_);
        // chassis_wheel_velocity_indicators_[2].set_value(*right_back_velocity_);
        // chassis_wheel_velocity_indicators_[3].set_value(*right_front_velocity_);

        // For test
        static uint8_t bullet{255};
        static uint32_t count{0};

        if (!(count++ % 30)) {
            status_ring_.update_bullet_allowance(bullet--);
        }

        status_ring_.update_battery_power(300 + 50 * std::sin((double)count / 300));
        status_ring_.update_friction_wheel_speed(300 + 20 * std::sin((double)count / 300), true);
        status_ring_.update_supercap(400 + 50 * std::sin((double)count / 200), true);
    }

private:
    void update_yaw_indicator() {
        yaw_indicator_.set_value(*supercap_voltage_);
        yaw_indicator_.set_center_x(x_center);
    }

    void update_chassis_direction_indicator() {
        auto chassis_mode = *chassis_mode_;

        auto to_referee_angle = [](double angle) {
            return static_cast<int>(std::round((2 * std::numbers::pi - angle) / std::numbers::pi * 180));
        };
        chassis_direction_indicator_.set_color(
            chassis_mode == rmcs_msgs::ChassisMode::SPIN ? Shape::Color::GREEN : Shape::Color::PINK);
        chassis_direction_indicator_.set_angle(to_referee_angle(*chassis_angle_), 30);

        bool chassis_control_direction_indicator_visible = false;
        if (!std::isnan(*chassis_control_angle_)) {
            if (chassis_mode == rmcs_msgs::ChassisMode::STEP_DOWN) {
                chassis_control_direction_indicator_visible = true;
                chassis_control_direction_indicator_.set_color(Shape::Color::CYAN);
                chassis_control_direction_indicator_.set_width(8);
                chassis_control_direction_indicator_.set_r(92);
                chassis_control_direction_indicator_.set_angle(to_referee_angle(*chassis_control_angle_), 30);
            } else if (chassis_mode == rmcs_msgs::ChassisMode::LAUNCH_RAMP) {
                chassis_control_direction_indicator_visible = true;
                chassis_control_direction_indicator_.set_color(Shape::Color::CYAN);
                chassis_control_direction_indicator_.set_width(28);
                chassis_control_direction_indicator_.set_r(102);
                chassis_control_direction_indicator_.set_angle(to_referee_angle(*chassis_control_angle_), 4);
            }
        }
        chassis_control_direction_indicator_.set_visible(chassis_control_direction_indicator_visible);
    }

    static constexpr uint16_t screen_width = 1920, screen_height = 1080;
    static constexpr uint16_t x_center = screen_width / 2, y_center = screen_height / 2;

    InputInterface<rmcs_msgs::ChassisMode> chassis_mode_;
    InputInterface<double> chassis_angle_, chassis_control_angle_;

    InputInterface<double> supercap_voltage_;
    InputInterface<bool> supercap_enabled_;

    InputInterface<double> chassis_voltage_;
    InputInterface<double> chassis_power_;
    InputInterface<double> chassis_control_power_limit_;
    InputInterface<double> supercap_control_power_limit_;

    InputInterface<double> left_front_velocity_, left_back_velocity_, right_back_velocity_,
        right_front_velocity_;

    InputInterface<uint16_t> robot_bullet_allowance_;

    Crosshair crosshair_;
    StatusRing status_ring_;

    Line horizontal_center_guidelines_[2];
    Line vertical_center_guidelines_[2];

    Float yaw_indicator_;
    Line yaw_indicator_guidelines_[2];

    Arc chassis_direction_indicator_, chassis_control_direction_indicator_;

    Float chassis_voltage_indicator_, chassis_power_indicator_, chassis_control_power_limit_indicator_,
        supercap_control_power_limit_indicator_;
    Float chassis_wheel_velocity_indicators_[4];
};

} // namespace rmcs_referee::app::ui

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::app::ui::Infantry, rmcs_executor::Component)