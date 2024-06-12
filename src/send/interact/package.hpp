#pragma once

#include "protocol/protocol.hpp"
#include "protocol/send.hpp"

#include <serial/serial.h>

#include <cassert>
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

} // namespace referee::ui

namespace referee {

} // namespace referee
