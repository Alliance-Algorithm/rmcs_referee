#pragma once

#include <cstdint>

#include "rmcs_referee/msg.hpp"

namespace rmcs_referee::command::interaction {

struct __attribute__((packed)) Header {
    uint16_t command_id;
    rmcs_referee::msg::Id sender_id;
    rmcs_referee::msg::Id receiver_id;
};

} // namespace rmcs_referee::command::interaction