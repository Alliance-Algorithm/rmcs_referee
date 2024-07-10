#pragma once

#include "app/ui/shape/shape.hpp"

namespace rmcs_referee::app::ui {

class CrossHair {
public:
    CrossHair(Shape::Color color, uint16_t x, uint16_t y)
        : guidelines_(
              {color, 2, static_cast<uint16_t>(x - r2), y, static_cast<uint16_t>(x - r1), y},
              {color, 2, static_cast<uint16_t>(x + r1), y, static_cast<uint16_t>(x + r2), y},
              {color, 2, x, static_cast<uint16_t>(y + r2), x, static_cast<uint16_t>(y + r1)},
              {color, 2, x, static_cast<uint16_t>(y - r1), x, static_cast<uint16_t>(y - r2)})
        , center_(color, 2, x, y, 1, 1) {}

    void set_visible(bool value) {
        for (auto& line : guidelines_)
            line.set_visible(value);
        center_.set_visible(value);
    }

private:
    static constexpr uint16_t r1 = 8, r2 = 24;

    Line guidelines_[4];
    Circle center_;
};

} // namespace rmcs_referee::app::ui