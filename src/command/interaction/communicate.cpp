#include <eigen3/Eigen/Eigen>

#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>
#include <robot_id.hpp>

#include "command/field.hpp"
#include "rmcs_executor/component.hpp"
#include "status/field.hpp"

namespace rmcs_referee::command::interaction {

class Communicate
    : public rmcs_executor::Component
    , public rclcpp::Node {

public:
    Communicate()
        : rclcpp::Node(get_component_name()) {
        register_input("/radar/enemies/position", enemies_position_);
        register_input("/referee/id", robot_id_);

        register_output("/referee/command/interaction/communicate", communicate_field_);
    }

    void update() override {
        if (!enemies_position_.ready())
            return;

        if (!robot_id_.ready() || *robot_id_ == rmcs_msgs::RobotId::UNKNOWN)
            return;

        *communicate_field_ = Field([this](std::byte* buffer) {
            struct __attribute__((packed)) Write {
                status::InteractionHeader header;
                status::EnemiesPosition position;
            };
            auto& write = *new (buffer) Write{};

            write.header.command = 0x222;
            if (robot_id_->color() == rmcs_msgs::RobotColor::RED) {
                write.header.sender   = 7;
                write.header.receiver = 9;
            } else {
                write.header.sender   = 107;
                write.header.receiver = 109;
            }

            write.position = *enemies_position_;

            return sizeof(Write);
        });
    }

private:
    InputInterface<status::EnemiesPosition> enemies_position_;
    InputInterface<rmcs_msgs::RobotId> robot_id_;

    OutputInterface<Field> communicate_field_;
};

} // namespace rmcs_referee::command::interaction

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::command::interaction::Communicate, rmcs_executor::Component)