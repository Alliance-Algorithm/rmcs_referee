#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>

#include "app/ui/shape/shape.hpp"

namespace rmcs_referee::app::ui {
using namespace std::chrono_literals;

class Infantry
    : public rmcs_executor::Component
    , public rclcpp::Node {
public:
    Infantry()
        : Node{
              get_component_name(),
              rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)} {

        for (auto& line : lines) {
            line.set_color(Shape::Color::BLACK);
            line.set_width(1);
            line.set_visible(true);
        }

        circle.set_color(Shape::Color::WHITE);
        circle.set_width(20);
        circle.set_x(1920 / 2);
        circle.set_y(1080 / 2);
        circle.set_r(450);
        circle.set_priority(1);
        circle.set_visible(true);
    }

    void update() override {
        // angle += 0.0001;
        double shift = 0;
        for (auto& line : lines) {
            double x = 400 * std::cos(angle + shift);
            double y = 400 * std::sin(angle + shift);
            line.set_x(static_cast<uint16_t>(std::round(1920 / 2.0 + x)));
            line.set_y(static_cast<uint16_t>(std::round(1080 / 2.0 + y)));
            line.set_x2(static_cast<uint16_t>(std::round(1920 / 2.0 - x)));
            line.set_y2(static_cast<uint16_t>(std::round(1080 / 2.0 - y)));
            shift += 2 * std::numbers::pi / 400;
        }
        auto now = std::chrono::steady_clock::now();
        if (tp_ < now) {
            tp_ = now + 1s;
            for (auto& line : lines) {
                line.set_color(static_cast<Shape::Color>(color_));
            }
            circle.set_color(static_cast<Shape::Color>(color_));
            if (++color_ > 8)
                color_ = 0;
        }
    }

private:
    double angle = 0;

    std::chrono::steady_clock::time_point tp_ = std::chrono::steady_clock::now() + 1s;

    Line lines[200];
    Circle circle;
    int color_ = 0;
};

} // namespace rmcs_referee::app::ui

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::app::ui::Infantry, rmcs_executor::Component)