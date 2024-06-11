#pragma once

#include "protocol/protocol.hpp"
#include "protocol/send.hpp"

#include <cstring>

namespace referee::ui {

using namespace referee::package;
using namespace referee::package::send;

using Description = interact::ui::Description;

using DeletePackage = Package<interact::Data<interact::ui::DeleteLayer>>;
using DrawPackage1  = Package<interact::Data<interact::ui::Description>>;
using DrawPackage2  = Package<interact::Data<interact::ui::Description2>>;
using DrawPackage5  = Package<interact::Data<interact::ui::Description5>>;
using DrawPackage7  = Package<interact::Data<interact::ui::Description7>>;
using StringPackage = Package<interact::Data<interact::ui::String>>;

enum class ShapeEnum {
    LINE,
    RECTANGLE,
    CIRCLE,
    ELLIPSE,
    ARC,
    FLOAT,
    INTEGER,
    STRING
};

enum class ColorEnum {
    SELF,
    YELLOW,
    GREEN,
    ORANGE,
    PURPLE_RED,
    PINK,
    BLUE,
    BLACK,
    WHITE
};
enum class OperationEnum {
    EMPTY,
    ADD,
    MODIFY,
    DELETE
};

class BasicDescription {
public:
    BasicDescription() = default;

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
        description.start_x   = x;
        description.start_y   = y;
        (void)description.graphic;
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

public:
    OperationEnum operation;
    ColorEnum color;
    uint8_t name[3];
    uint8_t layer;
    uint16_t width;
    uint16_t x;
    uint16_t y;
};

class Line : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::LINE);
        description.details_d = this->end_x;
        description.details_e = this->end_y;

        return description;
    }

public:
    uint16_t end_x;
    uint16_t end_y;
};

class Rectangle : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::RECTANGLE);
        description.details_d = this->other_x;
        description.details_e = this->other_y;

        return description;
    }

public:
    uint16_t other_x;
    uint16_t other_y;
};

class Circle : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::CIRCLE);
        description.details_c = radius;

        return description;
    }

public:
    uint16_t radius;
};

class Ellipse : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::ELLIPSE);
        description.details_d = radius_x;
        description.details_e = radius_y;

        return description;
    }

public:
    uint16_t radius_x;
    uint16_t radius_y;
};

class Arc : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::ARC);
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

class FLoat : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::FLOAT);
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

class Integer : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::INTEGER);
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

class String : public BasicDescription {
public:
    Description generate_description() override
    {
        auto description = generate_basic_description();

        description.graphic   = static_cast<uint8_t>(ShapeEnum::STRING);
        description.details_a = font_size;
        description.details_b = length;

        return description;
    }

public:
    uint16_t font_size;
    uint16_t length;
};
} // namespace referee::ui