#include <cstdint>

namespace rmcs_referee::msg {

enum class Color : uint8_t {
    UNKNOWN = 0,
    RED     = 1,
    BLUE    = 2,
};

enum class RobotId : uint8_t {
    UNKNOWN = 0,

    RED_HERO         = 1,
    RED_ENGINEER     = 2,
    RED_INFANTRY_III = 3,
    RED_INFANTRY_IV  = 4,
    RED_INFANTRY_V   = 5,
    RED_AERIAL       = 6,
    RED_SENTRY       = 7,
    RED_DART         = 8,
    RED_RADAR        = 9,
    RED_OUTPOST      = 10,
    RED_BASE         = 11,

    BLUE_HERO         = 101,
    BLUE_ENGINEER     = 102,
    BLUE_INFANTRY_III = 103,
    BLUE_INFANTRY_IV  = 104,
    BLUE_INFANTRY_V   = 105,
    BLUE_AERIAL       = 106,
    BLUE_SENTRY       = 107,
    BLUE_DART         = 108,
    BLUE_RADAR        = 109,
    BLUE_OUTPOST      = 110,
    BLUE_BASE         = 111,
};

// class RobotId {
// public:

//     constexpr RobotId()
//         : value_{UNKNOWN} {}
//     constexpr explicit RobotId(uint8_t value)
//         : value_{static_cast<Value>(value)} {}
//     constexpr RobotId(Value value)                      // NOLINT(google-explicit-constructor)
//         : value_{value} {}

//     constexpr operator Value() const { return value_; } // NOLINT(google-explicit-constructor)

//     constexpr RobotId& operator=(Value value) {
//         value_ = value;
//         return *this;
//     }
//     constexpr bool operator==(const Value value) const { return value_ == value; }
//     constexpr bool operator!=(const Value value) const { return value_ != value; }

//     constexpr Color color() { return value_ & 0x40 ? Color::BLUE : Color::RED; }

// private:
//     Value value_;
// };
enum class Id : uint16_t {
    UNKNOWN = 0,

    RED_HERO         = 1,
    RED_ENGINEER     = 2,
    RED_INFANTRY_III = 3,
    RED_INFANTRY_IV  = 4,
    RED_INFANTRY_V   = 5,
    RED_AERIAL       = 6,
    RED_SENTRY       = 7,
    RED_DART         = 8,
    RED_RADAR        = 9,
    RED_OUTPOST      = 10,
    RED_BASE         = 11,

    BLUE_HERO         = 101,
    BLUE_ENGINEER     = 102,
    BLUE_INFANTRY_III = 103,
    BLUE_INFANTRY_IV  = 104,
    BLUE_INFANTRY_V   = 105,
    BLUE_AERIAL       = 106,
    BLUE_SENTRY       = 107,
    BLUE_DART         = 108,
    BLUE_RADAR        = 109,
    BLUE_OUTPOST      = 110,
    BLUE_BASE         = 111,

    RED_HERO_CLIENT         = 0x0101,
    RED_ENGINEER_CLIENT     = 0x0102,
    RED_INFANTRY_III_CLIENT = 0x0103,
    RED_INFANTRY_IV_CLIENT  = 0x0104,
    RED_INFANTRY_V_CLIENT   = 0x0105,
    RED_AERIAL_CLIENT       = 0x0106,

    BLUE_HERO_CLIENT         = 0x0165,
    BLUE_ENGINEER_CLIENT     = 0x0166,
    BLUE_INFANTRY_III_CLIENT = 0x0167,
    BLUE_INFANTRY_IV_CLIENT  = 0x0168,
    BLUE_INFANTRY_V_CLIENT   = 0x0169,
    BLUE_AERIAL_CLIENT       = 0x016A,

    REFEREE_SERVER = 0x8080,
};

// class Id {
// public:

//     constexpr Id()
//         : value_{UNKNOWN} {}
//     constexpr explicit Id(uint16_t value)
//         : value_{static_cast<Value>(value)} {}
//     constexpr Id(Value value)                           // NOLINT(google-explicit-constructor)
//         : value_{value} {}
//     constexpr Id(RobotId::Value value)                  // NOLINT(google-explicit-constructor)
//         : value_{static_cast<Value>(static_cast<uint16_t>(value))} {}

//     constexpr operator Value() const { return value_; } // NOLINT(google-explicit-constructor)

//     constexpr Id& operator=(Value value) {
//         value_ = value;
//         return *this;
//     }
//     constexpr bool operator==(const Value value) const { return value_ == value; }
//     constexpr bool operator!=(const Value value) const { return value_ != value; }

//     constexpr Color color() {
//         if (value_ == REFEREE_SERVER)
//             return Color::UNKNOWN;
//         else [[likely]]
//             return value_ & 0x40 ? Color::BLUE : Color::RED;
//     }

// private:
//     Value value_;
// };

} // namespace rmcs_referee::msg