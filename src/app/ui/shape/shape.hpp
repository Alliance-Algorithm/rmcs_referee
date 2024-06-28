#pragma once

#include "cfs_scheduler.hpp"
#include "command/field.hpp"
#include "remote_shape.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <new>

namespace rmcs_referee {

namespace command::interaction {
class Ui;
}

namespace app::ui {

class Shape
    : private CfsScheduler<Shape>::Entity
    , private RemoteShape<Shape>::SwappableDescriptor {
public:
    friend class CfsScheduler<Shape>;
    friend class RemoteShape<Shape>;
    friend class command::interaction::Ui;

    bool visible() const { return visible_; }
    void set_visible(bool value) {
        if (visible_ == value)
            return;

        visible_ = value;

        // Optimizations
        if (!visible_) {
            if (existence_confidence() == 0) {
                // Simply leave run_queue when shape was hidden and remote shape does not exist.
                leave_run_queue();
                return;
            } else {
                // Otherwise enable swapping
                enable_swapping();
            }
        } else {
            // Disable swapping when shape is visible
            disable_swapping();
        }

        sync_confidence_ = 0;
        enter_run_queue();
    }

    uint8_t priority() const { return priority_; }
    void set_priority(uint8_t value) {
        if (priority_ == value)
            return;
        priority_ = value;
        if (is_in_run_queue())
            enter_run_queue();
    }

    uint16_t width() const { return part2_.width; }
    void set_width(uint16_t width) {
        if (part2_.width == width)
            return;
        part2_.width = width;
        set_modified();
    }

    uint16_t x() const { return part2_.x; }
    void set_x(uint16_t x) {
        if (part2_.x == x)
            return;
        part2_.x = x;
        set_modified();
    }

    uint16_t y() const { return part2_.y; }
    void set_y(uint16_t y) {
        if (part2_.y == y)
            return;
        part2_.y = y;
        set_modified();
    }

    void re_draw() { set_modified(); }

    bool is_text_shape() const { return is_text_shape_; }

    enum class Operation : uint8_t {
        NO_OPERATION = 0,
        ADD          = 1,
        MODIFY       = 2,
        DELETE       = 3,
    };

    Operation predict_update() const {
        uint8_t predict_existence = existence_confidence();
        uint8_t predict_sync      = sync_confidence_;

        if (!has_id() && !predict_try_assign_id(predict_existence)) {
            return Operation::NO_OPERATION;
        }

        if (predict_existence == 0) {
            predict_sync = max_update_times;
        }

        command::Field field;
        if (visible_
            && (predict_existence <= predict_sync
                || (last_time_modified_ && predict_existence < max_update_times))) {
            return Operation::ADD;
        } else {
            return Operation::MODIFY;
        }
    }

    constexpr static inline command::Field no_operation_description() {
        return command::Field{[](std::byte* buffer) {
            auto& description                = *new (buffer) DescriptionField{};
            description.part1.operation_type = static_cast<uint8_t>(Operation::NO_OPERATION);
            return sizeof(DescriptionField);
        }};
    }

    enum class Color : uint8_t {
        SELF   = 0,
        YELLOW = 1,
        GREEN  = 2,
        ORANGE = 3,
        PURPLE = 4,
        PINK   = 5,
        CYAN   = 6,
        BLACK  = 7,
        WHITE  = 8,
    };

protected:
    struct DescriptionField {
        uint8_t name[3];
        struct __attribute__((packed)) Part1 {
            uint8_t operation_type : 3;
            uint8_t shape_type     : 3;
            uint8_t layer          : 4;
            uint8_t color          : 4;
            uint16_t details_a     : 9;
            uint16_t details_b     : 9;
        } part1;
        struct __attribute__((packed)) Part2 {
            uint16_t width : 10;
            uint16_t x     : 11;
            uint16_t y     : 11;
        } part2;
        struct __attribute__((packed)) Part3 {
            uint16_t details_c : 10;
            uint16_t details_d : 11;
            uint16_t details_e : 11;
        } part3;
    };

    void set_modified() {
        // Optimization: Assume the modification not exist when invisible.
        if (!visible_)
            return;

        sync_confidence_ = 0;
        enter_run_queue();
    }

    virtual size_t write_description_field(std::byte* buffer) = 0;

    DescriptionField::Part2 part2_ alignas(4);

private:
    void enter_run_queue() {
        uint8_t min_confidence     = std::min(existence_confidence(), sync_confidence_);
        uint16_t weighted_priority = (priority_ - 256) << (4 * min_confidence);
        CfsScheduler<Shape>::Entity::enter_run_queue(weighted_priority);
    }

    void after_swapped() {
        // Called by RemoteShape<Shape>::SwappableDescriptor
        // Leave run_queue when remote shape was swapped.
        leave_run_queue();
    }

    command::Field update() {
        if (!has_id() && !try_assign_id()) {
            // TODO: Print error message.
            sync_confidence_ = max_update_times;
            visible_         = false;
            // Do nothing when failed
            return no_operation_description();
        }

        if (existence_confidence() == 0) {
            // Optimization: Always consider it synchronized when remote shape does not exist.
            sync_confidence_ = max_update_times;
        }

        command::Field field;

        // Optimization1: Stop adding when shape is invisible.
        // Optimization2: Prevent continuous modification.
        if (visible_
            && (existence_confidence() <= sync_confidence_
                || (last_time_modified_ && existence_confidence() < max_update_times))) {
            // Send add packet
            last_time_modified_ = false;
            field               = command::Field{[this](std::byte* buffer) {
                return write_full_description_field(buffer, Operation::ADD);
            }};
            if (increase_existence_confidence() < max_update_times
                || sync_confidence_ < max_update_times)
                enter_run_queue();
        } else {
            // Send modify packet
            last_time_modified_ = true;
            field               = command::Field{[this](std::byte* buffer) {
                return write_full_description_field(buffer, Operation::MODIFY);
            }};
            // No need to compare existence_confidence here.
            // Because either the shape is not visible here, no need to send add packet.
            // Or existence_confidence > sync_confidence, only the min value needs to be considered.
            if (++sync_confidence_ < max_update_times)
                enter_run_queue();
        }

        return field;
    }

    size_t write_full_description_field(std::byte* buffer, Operation operation) {
        auto written      = write_description_field(buffer);
        auto& description = *std::launder(reinterpret_cast<DescriptionField*>(buffer));

        // No special meaning, just to ensure no duplication
        description.name[0] = id();
        description.name[1] = 0xef;
        description.name[2] = 0xfe;

        // We only use layer 0
        description.part1.layer = 0;

        description.part1.operation_type = static_cast<uint8_t>(operation);

        return written;
    }

    static constexpr uint8_t max_update_times = 4;

    uint8_t priority_            = 0;
    uint8_t sync_confidence_ : 5 = max_update_times;
    bool is_text_shape_      : 1 = false;
    bool last_time_modified_ : 1 = false;
    bool visible_            : 1 = false;
};

class Line : public Shape {
public:
    Line() = default;
    Line(Color color, uint16_t width, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {
        part3_.color = color;
        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;
        part3_.x2    = x2;
        part3_.y2    = y2;
    }

    Color color() const { return part3_.color; }
    void set_color(Color color) {
        if (part3_.color == color)
            return;
        part3_.color = color;
        set_modified();
    }

    uint16_t x2() const { return part3_.x2; }
    void set_x2(uint16_t x2) {
        if (part3_.x2 == x2)
            return;
        part3_.x2 = x2;
        set_modified();
    }

    uint16_t y2() const { return part3_.y2; }
    void set_y2(uint16_t y2) {
        if (part3_.y2 == y2)
            return;
        part3_.y2 = y2;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1.color      = static_cast<uint8_t>(part3_.color);
        description.part1.shape_type = 0; // Line

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    struct __attribute__((packed)) {
        // Since details_c is invalid in lines, the memory is used to store color
        Color color         : 8;
        uint8_t placeholder : 2;

        uint16_t x2 : 11;
        uint16_t y2 : 11;
    } part3_ alignas(4);
};

class Circle : public Shape {
public:
    Circle() = default;
    Circle(Color color, uint16_t width, uint16_t x, uint16_t y, uint16_t rx, uint16_t ry) {
        part3_.color = color;
        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;
        part3_.rx    = rx;
        part3_.ry    = ry;
    }

    Color color() const { return part3_.color; }
    void set_color(Color color) {
        if (part3_.color == color)
            return;
        part3_.color = color;
        set_modified();
    }

    void set_r(uint16_t r) {
        set_rx(r);
        set_ry(r);
    }

    uint16_t rx() const { return part3_.rx; }
    void set_rx(uint16_t rx) {
        if (part3_.rx == rx)
            return;
        part3_.rx = rx;
        set_modified();
    }

    uint16_t ry() const { return part3_.ry; }
    void set_ry(uint16_t ry) {
        if (part3_.ry == ry)
            return;
        part3_.ry = ry;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1.color      = static_cast<uint8_t>(part3_.color);
        description.part1.shape_type = 3; // Only use type ellipse

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    struct __attribute__((packed)) {
        Color color         : 8;
        uint8_t placeholder : 2;

        uint16_t rx : 11;
        uint16_t ry : 11;
    } part3_ alignas(4);
};

class Rectangle : public Shape {
public:
    Rectangle() = default;
    Rectangle(Color color, uint16_t width, uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {
        part3_.color = color;
        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;
        part3_.x2    = x2;
        part3_.y2    = y2;
    }

    Color color() const { return part3_.color; }
    void set_color(Color color) {
        if (part3_.color == color)
            return;
        part3_.color = color;
        set_modified();
    }

    uint16_t x2() const { return part3_.x2; }
    void set_x2(uint16_t x2) {
        if (part3_.x2 == x2)
            return;
        part3_.x2 = x2;
        set_modified();
    }

    uint16_t y2() const { return part3_.y2; }
    void set_y2(uint16_t y2) {
        if (part3_.y2 == y2)
            return;
        part3_.y2 = y2;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1.color      = static_cast<uint8_t>(part3_.color);
        description.part1.shape_type = 1; // Rectangle

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    struct __attribute__((packed)) {
        Color color         : 8;
        uint8_t placeholder : 2;

        uint16_t x2 : 11;
        uint16_t y2 : 11;
    } part3_ alignas(4);
};

class Arc : public Shape {
public:
    Arc() { part1_.shape_type = 4; };
    Arc(Color color, uint16_t width, uint16_t x, uint16_t y, uint16_t angle_start,
        uint16_t angle_end, uint16_t rx, uint16_t ry)
        : Arc() {
        part1_.color     = static_cast<uint8_t>(color);
        part1_.details_a = angle_start;
        part1_.details_b = angle_end;

        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;

        part3_.rx = rx;
        part3_.ry = ry;
    }

    Color color() const { return static_cast<Color>(part1_.color); }
    void set_color(Color color) {
        if (part1_.color == static_cast<uint8_t>(color))
            return;
        part1_.color = static_cast<uint8_t>(color);
        set_modified();
    }

    uint16_t angle_start() const { return part1_.details_a; }
    void set_angle_start(uint16_t angle_start) {
        if (part1_.details_a == angle_start)
            return;
        part1_.details_a = angle_start;
        set_modified();
    }

    uint16_t angle_end() const { return part1_.details_b; }
    void set_angle_end(uint16_t angle_end) {
        if (part1_.details_b == angle_end)
            return;
        part1_.details_b = angle_end;
        set_modified();
    }

    uint16_t rx() const { return part3_.rx; }
    void set_rx(uint16_t rx) {
        if (part3_.rx == rx)
            return;
        part3_.rx = rx;
        set_modified();
    }

    uint16_t ry() const { return part3_.ry; }
    void set_ry(uint16_t ry) {
        if (part3_.ry == ry)
            return;
        part3_.ry = ry;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1 = part1_;

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    DescriptionField::Part1 part1_;
    struct __attribute__((packed)) {
        uint16_t placeholder : 10;

        uint16_t rx : 11;
        uint16_t ry : 11;
    } part3_ alignas(4);
};

class Float : public Shape {
public:
    Float() { part1_.shape_type = 5; };
    Float(Color color, uint16_t width, uint16_t x, uint16_t y, int32_t value, uint16_t font_size)
        : Float() {
        part1_.color     = static_cast<uint8_t>(color);
        part1_.details_a = font_size;

        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;

        part3_.value = value;
    }

    Color color() const { return static_cast<Color>(part1_.color); }
    void set_color(Color color) {
        if (part1_.color == static_cast<uint8_t>(color))
            return;
        part1_.color = static_cast<uint8_t>(color);
        set_modified();
    }

    int32_t value() const { return part3_.value; }
    void set_value(int32_t value) {
        if (part3_.value == value)
            return;
        part3_.value = value;
        set_modified();
    }

    uint16_t font_size() const { return part1_.details_a; }
    void set_font_size(uint16_t font_size) {
        if (part1_.details_a == font_size)
            return;
        part1_.details_a = font_size;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1 = part1_;

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    DescriptionField::Part1 part1_;
    struct __attribute__((packed)) {
        int32_t value : 32; // float = value / 1000
    } part3_ alignas(4);
};

class Integer : public Shape {
public:
    Integer() { part1_.shape_type = 6; };
    Integer(Color color, uint16_t width, uint16_t x, uint16_t y, int32_t value, uint16_t font_size)
        : Integer() {
        part1_.color     = static_cast<uint8_t>(color);
        part1_.details_a = font_size;

        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;

        part3_.value = value;
    }

    Color color() const { return static_cast<Color>(part1_.color); }
    void set_color(Color color) {
        if (part1_.color == static_cast<uint8_t>(color))
            return;
        part1_.color = static_cast<uint8_t>(color);
        set_modified();
    }

    int32_t value() const { return part3_.value; }
    void set_value(int32_t value) {
        if (part3_.value == value)
            return;
        part3_.value = value;
        set_modified();
    }

    uint16_t font_size() const { return part1_.details_a; }
    void set_font_size(uint16_t font_size) {
        if (part1_.details_a == font_size)
            return;
        part1_.details_a = font_size;
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1 = part1_;

        description.part2 = part2_;

        description.part3 = std::bit_cast<DescriptionField::Part3>(part3_);

        return sizeof(DescriptionField);
    }

private:
    DescriptionField::Part1 part1_;
    struct __attribute__((packed)) {
        int32_t value : 32;
    } part3_ alignas(4);
};

class String : public Shape {
public:
    String() { part1_.shape_type = 7; };
    String(Color color, uint16_t width, uint16_t x, uint16_t y, uint16_t font_size)
        : String() {
        part1_.color     = static_cast<uint8_t>(color);
        part1_.details_a = font_size;

        part2_.width = width;
        part2_.x     = x;
        part2_.y     = y;
    }

    Color color() const { return static_cast<Color>(part1_.color); }
    void set_color(Color color) {
        if (part1_.color == static_cast<uint8_t>(color))
            return;
        part1_.color = static_cast<uint8_t>(color);
        set_modified();
    }

    uint16_t font_size() const { return part1_.details_a; }
    void set_font_size(uint16_t font_size) {
        if (part1_.details_a == font_size)
            return;
        part1_.details_a = font_size;
        set_modified();
    }

    uint16_t length() const { return part1_.details_b; }
    char* string() const { return const_cast<char*>(string_); }
    void set_string(char* string, uint16_t length) {
        if (std::strncmp(string_, string, length) == 0)
            return;
        part1_.details_b = length;
        std::strncpy(string_, string, length);
        set_modified();
    }

protected:
    size_t write_description_field(std::byte* buffer) override {
        auto& description = *new (buffer) DescriptionField{};

        description.part1 = part1_;

        description.part2 = part2_;

        std::memcpy(buffer + sizeof(DescriptionField), string_, length());

        return sizeof(DescriptionField);
    }

private:
    DescriptionField::Part1 part1_;
    char string_[30];
};

} // namespace app::ui
} // namespace rmcs_referee