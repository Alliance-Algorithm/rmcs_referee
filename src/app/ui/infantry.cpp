#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>

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
              {Shape::Color::WHITE, 2, x_center - 360, y_center, x_center - 100, y_center},
              {Shape::Color::WHITE, 2, x_center + 100, y_center, x_center + 360, y_center})
        , vertical_center_guidelines_(
              {Shape::Color::WHITE, 2, x_center, 800, x_center, y_center + 100},
              {Shape::Color::WHITE, 2, x_center, y_center - 100, x_center, 200})
        , yaw_indicator_(Shape::Color::WHITE, 20, 2, x_center - 10, 860, 0)
        , yaw_indicator_guidelines_(
              {Shape::Color::WHITE, 2, x_center - 32, 830, x_center + 32, 830},
              {Shape::Color::WHITE, 2, x_center, 830, x_center, 820})
        , chassis_direction_indicator_(Shape::Color::PINK, 8, x_center, y_center, 0, 0, 67, 67) {

        crosshair_.set_visible(true);

        horizontal_center_guidelines_[0].set_visible(true);
        horizontal_center_guidelines_[1].set_visible(true);
        vertical_center_guidelines_[0].set_visible(true);
        vertical_center_guidelines_[1].set_visible(true);

        yaw_indicator_.set_visible(true);
        yaw_indicator_guidelines_[0].set_visible(true);
        yaw_indicator_guidelines_[1].set_visible(true);

        chassis_direction_indicator_.set_visible(true);
    }

    void update() override {
        angle_ += 0.001;
        if (angle_ >= 2 * std::numbers::pi)
            angle_ -= 2 * std::numbers::pi;
        update_yaw_indicator(angle_);
        update_chassis_direction_indicator(angle_);
    }

private:
    void update_yaw_indicator(double yaw) {
        int yaw_degree       = static_cast<int>(std::round(yaw / std::numbers::pi * 180));
        int number_of_digits = 0;
        for (int n = yaw_degree; n != 0; number_of_digits++)
            n /= 10;
        yaw_indicator_.set_value(yaw_degree);
        yaw_indicator_.set_x(x_center - 10 * number_of_digits + 3);
    }

    void update_chassis_direction_indicator(double angle) {
        int middle =
            static_cast<int>(std::round((2 * std::numbers::pi - angle) / std::numbers::pi * 180));

        int start = middle - 30;
        if (start < 0)
            start += 360;
        chassis_direction_indicator_.set_angle_start(start);

        int end = middle + 30;
        if (end >= 360)
            end -= 360;
        chassis_direction_indicator_.set_angle_end(end);
    }

    static constexpr uint16_t screen_width = 1920, screen_height = 1080;
    static constexpr uint16_t x_center = screen_width / 2, y_center = screen_height / 2;

    double angle_ = 0;

    CrossHair crosshair_;

    Line horizontal_center_guidelines_[2];
    Line vertical_center_guidelines_[2];

    Integer yaw_indicator_;
    Line yaw_indicator_guidelines_[2];

    Arc chassis_direction_indicator_;
};

} // namespace rmcs_referee::app::ui

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::app::ui::Infantry, rmcs_executor::Component)