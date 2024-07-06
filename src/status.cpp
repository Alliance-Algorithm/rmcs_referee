#include <rclcpp/node.hpp>

#include <rmcs_executor/component.hpp>
#include <rmcs_msgs/game_stage.hpp>
#include <rmcs_msgs/robot_id.hpp>
#include <serial_util/crc/dji_crc.hpp>
#include <serial_util/package_receive.hpp>
#include <serial_util/tick_timer.hpp>

#include "frame.hpp"
#include "status/field.hpp"

namespace rmcs_referee {
using namespace status;

class Status
    : public rmcs_executor::Component
    , public rclcpp::Node {
public:
    Status()
        : Node{get_component_name(), rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)}
        , logger_(get_logger()) {

        try {
            register_output(
                "/referee/serial", serial_, get_parameter("path").as_string(), 115200,
                serial::Timeout::simpleTimeout(0));
        } catch (serial::IOException& ex) {
            RCLCPP_ERROR(logger_, "Unable to open serial port: %s", ex.what());
        }

        register_output("/referee/game/stage", game_stage_, rmcs_msgs::GameStage::UNKNOWN);

        register_output("/referee/id", robot_id_, rmcs_msgs::RobotId::UNKNOWN);
        register_output("/referee/shooter/cooling", robot_shooter_cooling_, 0);
        register_output("/referee/shooter/heat_limit", robot_shooter_heat_limit_, 0);
        register_output("/referee/chassis_power_limit", robot_chassis_power_limit_, 0.0);

        register_output("/referee/chassis_power", robot_chassis_power_, 0.0);
        register_output("/referee/buffer_energy", robot_buffer_energy_, 60.0);

        robot_status_watchdog_.reset(5'000);
    }

    void update() override {
        if (!serial_.active())
            return;

        if (cache_size_ >= sizeof(frame_.header)) {
            auto frame_size = sizeof(frame_.header) + sizeof(frame_.body.command_id)
                            + frame_.header.data_length + sizeof(uint16_t);
            cache_size_ += serial_->read(
                reinterpret_cast<uint8_t*>(&frame_) + cache_size_, frame_size - cache_size_);

            if (cache_size_ == frame_size) {
                cache_size_ = 0;
                if (serial_util::dji_crc::verify_crc16(&frame_, frame_size)) {
                    process_frame();
                } else {
                    RCLCPP_WARN(logger_, "Body crc16 invalid");
                }
            }
        } else {
            auto result = serial_util::receive_package(
                *serial_, frame_.header, cache_size_, static_cast<uint8_t>(0xa5),
                [](const FrameHeader& header) {
                    return serial_util::dji_crc::verify_crc8(header);
                });
            if (result == serial_util::ReceiveResult::HEADER_INVALID) {
                RCLCPP_WARN(logger_, "Header start invalid");
            } else if (result == serial_util::ReceiveResult::VERIFY_INVALID) {
                RCLCPP_WARN(logger_, "Header crc8 invalid");
            }
        }

        if (game_status_watchdog_.tick()) {
            RCLCPP_INFO(logger_, "Game status receiving timeout. Set stage to unknown.");
            *game_stage_ = rmcs_msgs::GameStage::UNKNOWN;
        }
        if (robot_status_watchdog_.tick()) {
            RCLCPP_ERROR(logger_, "Robot status receiving timeout. Set to safe indicators.");
            *robot_shooter_cooling_     = safe_shooter_cooling;
            *robot_shooter_heat_limit_  = safe_shooter_heat_limit;
            *robot_chassis_power_limit_ = safe_chassis_power_limit;
        }
        if (power_heat_data_watchdog_.tick()) {
            RCLCPP_ERROR(logger_, "Power heat data receiving timeout. Set to initial values.");
            *robot_chassis_power_ = 0.0;
            *robot_buffer_energy_ = 60.0;
        }
    }

private:
    void process_frame() {
        auto command_id = frame_.body.command_id;
        if (command_id == 0x0001)
            update_game_status();
        if (command_id == 0x0003)
            update_game_robot_hp();
        else if (command_id == 0x0201)
            update_robot_status();
        else if (command_id == 0x0202)
            update_power_heat_data();
        else if (command_id == 0x0203)
            update_robot_position();
        else if (command_id == 0x0206)
            update_hurt_data();
        else if (command_id == 0x0207)
            update_shoot_data();
        else if (command_id == 0x0208)
            update_bullet_allowance();
        else if (command_id == 0x020B)
            update_game_robot_position();
    }

    void update_game_status() {
        auto& data = reinterpret_cast<GameStatus&>(frame_.body.data);

        *game_stage_ = static_cast<rmcs_msgs::GameStage>(data.game_stage);
        if (*game_stage_ == rmcs_msgs::GameStage::STARTED)
            game_status_watchdog_.reset(30'000);
        else
            game_status_watchdog_.reset(5'000);
    }

    void update_game_robot_hp() {}

    void update_robot_status() {
        if (*game_stage_ == rmcs_msgs::GameStage::STARTED)
            robot_status_watchdog_.reset(60'000);
        else
            robot_status_watchdog_.reset(5'000);

        auto& data = reinterpret_cast<RobotStatus&>(frame_.body.data);

        *robot_id_                  = static_cast<rmcs_msgs::RobotId>(data.robot_id);
        *robot_shooter_cooling_     = data.shooter_barrel_cooling_value;
        *robot_shooter_heat_limit_  = static_cast<int64_t>(1000) * data.shooter_barrel_heat_limit;
        *robot_chassis_power_limit_ = static_cast<double>(data.chassis_power_limit);
    }

    void update_power_heat_data() {
        power_heat_data_watchdog_.reset(3'000);

        auto& data            = reinterpret_cast<PowerHeatData&>(frame_.body.data);
        *robot_chassis_power_ = data.chassis_power;
        *robot_buffer_energy_ = static_cast<double>(data.buffer_energy);
    }

    void update_robot_position() {}

    void update_hurt_data() {}

    void update_shoot_data() {}

    void update_bullet_allowance() {}

    void update_game_robot_position() {}

    // When referee system loses connection unexpectedly,
    // use these indicators make sure the robot safe.
    // Muzzle: Cooling priority with level 1
    static constexpr int64_t safe_shooter_cooling    = 40;
    static constexpr int64_t safe_shooter_heat_limit = 50'000;
    // Chassis: Health priority with level 1
    static constexpr double safe_chassis_power_limit = 45;

    rclcpp::Logger logger_;

    OutputInterface<serial::Serial> serial_;
    Frame frame_;
    size_t cache_size_ = 0;

    serial_util::TickTimer game_status_watchdog_;
    OutputInterface<rmcs_msgs::GameStage> game_stage_;

    serial_util::TickTimer robot_status_watchdog_;
    OutputInterface<rmcs_msgs::RobotId> robot_id_;
    OutputInterface<int64_t> robot_shooter_cooling_, robot_shooter_heat_limit_;
    OutputInterface<double> robot_chassis_power_limit_;

    serial_util::TickTimer power_heat_data_watchdog_;
    OutputInterface<double> robot_chassis_power_;
    OutputInterface<double> robot_buffer_energy_;
};

} // namespace rmcs_referee

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::Status, rmcs_executor::Component)