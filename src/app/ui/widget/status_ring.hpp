#pragma once

#include "app/ui/shape/shape.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace rmcs_referee::app::ui {

class StatusRing {
public:
    StatusRing() {
        supercap_status_.set_x(x_center);
        supercap_status_.set_y(y_center);
        supercap_status_.set_r(visible_radius - width_ring);
        supercap_status_.set_angle_start(265 - visible_angle);
        supercap_status_.set_angle_end(265);
        supercap_status_.set_width(width_ring);
        supercap_status_.set_visible(true);
        supercap_status_.set_color(Shape::Color::PINK);

        battery_power_.set_x(x_center);
        battery_power_.set_y(y_center);
        battery_power_.set_r(visible_radius - width_ring);
        battery_power_.set_angle_start(275);
        battery_power_.set_angle_end(275 + visible_angle);
        battery_power_.set_width(width_ring);
        battery_power_.set_visible(true);
        battery_power_.set_color(Shape::Color::PINK);

        friction_wheel_speed_.set_x(x_center);
        friction_wheel_speed_.set_y(y_center);
        friction_wheel_speed_.set_r(visible_radius - width_ring);
        friction_wheel_speed_.set_angle_start(85 - visible_angle);
        friction_wheel_speed_.set_angle_end(85);
        friction_wheel_speed_.set_width(width_ring);
        friction_wheel_speed_.set_visible(true);
        friction_wheel_speed_.set_color(Shape::Color::PINK);

        bullet_allowance_.set_x(x_center);
        bullet_allowance_.set_y(y_center);
        bullet_allowance_.set_r(visible_radius - width_ring);
        bullet_allowance_.set_angle_start(95);
        bullet_allowance_.set_angle_end(95 + visible_angle);
        bullet_allowance_.set_width(width_ring);
        bullet_allowance_.set_visible(true);
        bullet_allowance_.set_color(Shape::Color::PINK);

        // UI
        line_left_center_.set_x(x_center - visible_radius);
        line_left_center_.set_x2(x_center - visible_radius + width_ring + 10);
        line_left_center_.set_y(y_center);
        line_left_center_.set_y2(y_center);
        line_left_center_.set_width(40);
        line_left_center_.set_color(Shape::Color::WHITE);
        line_left_center_.set_visible(true);

        line_right_center_.set_x(x_center + visible_radius);
        line_right_center_.set_x2(x_center + visible_radius - width_ring - 10);
        line_right_center_.set_y(y_center);
        line_right_center_.set_y2(y_center);
        line_right_center_.set_width(40);
        line_right_center_.set_color(Shape::Color::WHITE);
        line_right_center_.set_visible(true);

        arc_left_up_.set_x(x_center);
        arc_left_up_.set_y(y_center);
        arc_left_up_.set_r(visible_radius - width_ring);
        arc_left_up_.set_angle_start(275 + visible_angle + 1);
        arc_left_up_.set_angle_end(275 + visible_angle + 3);
        arc_left_up_.set_width(width_ring + 10);
        arc_left_up_.set_visible(true);
        arc_left_up_.set_color(Shape::Color::WHITE);

        arc_left_down_.set_x(x_center);
        arc_left_down_.set_y(y_center);
        arc_left_down_.set_r(visible_radius - width_ring);
        arc_left_down_.set_angle_start(265 - visible_angle - 3);
        arc_left_down_.set_angle_end(265 - visible_angle - 1);
        arc_left_down_.set_width(width_ring + 10);
        arc_left_down_.set_visible(true);
        arc_left_down_.set_color(Shape::Color::WHITE);

        arc_right_up_.set_x(x_center);
        arc_right_up_.set_y(y_center);
        arc_right_up_.set_r(visible_radius - width_ring);
        arc_right_up_.set_angle_start(85 - visible_angle - 3);
        arc_right_up_.set_angle_end(85 - visible_angle - 1);
        arc_right_up_.set_width(width_ring + 10);
        arc_right_up_.set_visible(true);
        arc_right_up_.set_color(Shape::Color::WHITE);

        arc_right_down_.set_x(x_center);
        arc_right_down_.set_y(y_center);
        arc_right_down_.set_r(visible_radius - width_ring);
        arc_right_down_.set_angle_start(95 + visible_angle + 1);
        arc_right_down_.set_angle_end(95 + visible_angle + 3);
        arc_right_down_.set_width(width_ring + 10);
        arc_right_down_.set_visible(true);
        arc_right_down_.set_color(Shape::Color::WHITE);

        set_limits(1000, 1000, 1000, 1000);
    }

    void update_supercap(double value, bool enable) {
        value = std::clamp(value, double(0), supercap_limit_);

        auto angle = 265 - visible_angle * value / supercap_limit_ - 1;
        supercap_status_.set_angle_start(static_cast<uint16_t>(angle));

        if (enable) {
            supercap_status_.set_color(Shape::Color::GREEN);
        } else {
            supercap_status_.set_color(Shape::Color::PINK);
        }
    }

    void update_battery_power(double value) {
        value = std::clamp(value, double(0), battery_limit_);

        auto angle = 275 + visible_angle * value / battery_limit_ + 1;
        battery_power_.set_angle_end(static_cast<uint16_t>(angle));
    }

    void update_friction_wheel_speed(double value, bool enable) {
        value = std::clamp(value, double(0), friction_limit_);

        auto angle = 85 - visible_angle * value / friction_limit_ - 1;
        friction_wheel_speed_.set_angle_start(static_cast<uint16_t>(angle));

        if (enable) {
            friction_wheel_speed_.set_color(Shape::Color::GREEN);
        } else {
            friction_wheel_speed_.set_color(Shape::Color::PINK);
        }
    }

    // @note bullet allowance should more than zero
    void update_bullet_allowance(uint16_t value) {
        value = std::clamp(value, uint16_t(0), bullet_limit_);

        auto angle = 95 + visible_angle * value / bullet_limit_ + 1;
        bullet_allowance_.set_angle_end(static_cast<uint16_t>(angle));

        if (value < 50) {
            bullet_allowance_.set_color(Shape::Color::PINK);
        } else {
            bullet_allowance_.set_color(Shape::Color::GREEN);
        }
    }

    void set_limits(
        double supercap_limit, double battery_limit, double friction_limit, uint16_t bullet_limit) {
        supercap_limit_ = supercap_limit;
        battery_limit_  = battery_limit;
        friction_limit_ = friction_limit;
        bullet_limit_   = bullet_limit;

        int scale_angle = 0;
        for (auto& bullet_scale : bullet_scales_) {

            scale_angle += (visible_angle + 10) / 4;

            bullet_scale.set_x(x_center);
            bullet_scale.set_y(y_center);
            bullet_scale.set_r(visible_radius - width_ring - 30);
            bullet_scale.set_angle_start(90 + scale_angle - 1);
            bullet_scale.set_angle_end(90 + scale_angle);
            bullet_scale.set_width(width_ring + 10);
            bullet_scale.set_color(Shape::Color::WHITE);
            bullet_scale.set_visible(true);
        }

        double value = 0;
        scale_angle  = 0;
        for (auto& number : bullet_scale_numbers_) {

            scale_angle += (visible_angle + 10) / 4;
            value += static_cast<double>(bullet_limit_) / 4;

            const auto r     = visible_radius - width_ring + 30;
            const auto angle = static_cast<double>(-scale_angle) * std::numbers::pi / 180;

            number.set_x(x_center + static_cast<uint16_t>(r * std::cos(angle)));
            number.set_y(y_center + static_cast<uint16_t>(r * std::sin(angle)));
            number.set_color(Shape::Color::WHITE);
            number.set_font_size(15);
            number.set_width(2);
            number.set_value(static_cast<uint16_t>(value));
            number.set_visible(true);
        }
    }

    // void set_visible(bool value) {}

private:
    constexpr static uint16_t x_center       = 960;
    constexpr static uint16_t y_center       = 540;
    constexpr static uint16_t width_ring     = 15;
    constexpr static uint16_t visible_radius = 400;
    constexpr static uint16_t visible_angle  = 40;

    double supercap_limit_;
    double battery_limit_;
    double friction_limit_;
    uint16_t bullet_limit_;

    // Status
    Arc supercap_status_;

    Arc battery_power_;

    Arc friction_wheel_speed_;

    Arc bullet_allowance_;
    Arc bullet_scales_[4];
    Integer bullet_scale_numbers_[4];

    // UI
    Line line_left_center_;
    Line line_right_center_;
    Arc arc_left_up_;
    Arc arc_left_down_;
    Arc arc_right_up_;
    Arc arc_right_down_;
};

} // namespace rmcs_referee::app::ui