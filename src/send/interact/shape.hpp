#pragma once

#include "send/interact/package.hpp"

#include <cassert>
#include <cstring>
#include <string>

namespace referee::ui {
class Shape {
public:
    OperationEnum operation;
    ColorEnum color;
    uint8_t name[3];
    uint8_t layer;
    uint16_t width;
    uint16_t x;
    uint16_t y;

    std::string string;

public:
    Shape() = default;

    virtual Description generate_description() = 0;

    Description generate_basic_description()
    {
        auto description = Description();

        description.name[0]   = name[0];
        description.name[1]   = name[1];
        description.name[2]   = name[2];
        description.operation = static_cast<uint32_t>(operation);
        description.layer     = layer;
        description.color     = static_cast<uint32_t>(color);
        description.width     = width;
        description.x_start   = x;
        description.y_start   = y;
        (void)description.shape;
        (void)description.details_a;
        (void)description.details_b;
        (void)description.details_c;
        (void)description.details_d;
        (void)description.details_e;

        return description;
    }

    void set_name(const char str[3])
    {
        std::strncpy(reinterpret_cast<char*>(name), str, sizeof(name));
    }
};

class Line : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::LINE);
        description.details_d = this->x_end;
        description.details_e = this->y_end;

        return description;
    }

public:
    uint16_t x_end;
    uint16_t y_end;
};

class Rectangle : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::RECTANGLE);
        description.details_d = this->x_end;
        description.details_e = this->y_end;

        return description;
    }

public:
    uint16_t x_end;
    uint16_t y_end;
};

class Circle : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::CIRCLE);
        description.details_c = radius;

        return description;
    }

public:
    uint16_t radius;
};

class Ellipse : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::ELLIPSE);
        description.details_d = radius_x;
        description.details_e = radius_y;

        return description;
    }

public:
    uint16_t radius_x;
    uint16_t radius_y;
};

class Arc : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::ARC);
        description.details_d = radius_x;
        description.details_e = radius_y;

        return description;
    }

public:
    uint16_t angle_start;
    uint16_t angle_end;
    uint16_t radius_x;
    uint16_t radius_y;
};

class Float : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::FLOAT);
        description.details_a = font_size;
        description.details_c = va_dot_lue >> 22;
        description.details_d = va_dot_lue >> 11;
        description.details_e = va_dot_lue >> 00;

        return description;
    }

public:
    uint16_t font_size;
    uint32_t va_dot_lue;
};

class Integer : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::INTEGER);
        description.details_a = font_size;
        description.details_c = value >> 00;
        description.details_d = value >> 11;
        description.details_e = value >> 22;

        return description;
    }

public:
    uint16_t font_size;
    int32_t value;
};

class String : public Shape {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.shape     = static_cast<uint8_t>(ShapeEnum::STRING);
        description.details_a = font_size;
        description.details_b = length;

        return description;
    }

public:
    uint16_t font_size;
    uint16_t length;
};
} // namespace referee::ui