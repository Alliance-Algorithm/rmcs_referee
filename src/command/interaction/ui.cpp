#include <algorithm>

#include <rclcpp/node.hpp>
#include <rmcs_executor/component.hpp>

#include "app/ui/shape/cfs_scheduler.hpp"
#include "app/ui/shape/shape.hpp"
#include "command/interaction/header.hpp"

namespace rmcs_referee::command::interaction {

class Ui
    : public rmcs_executor::Component
    , public rclcpp::Node {
public:
    Ui()
        : Node{
              get_component_name(),
              rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true)} {
        register_output("/referee/command/interaction/ui", ui_field_);
    }

    void update() override {
        using namespace app::ui;
        if (CfsScheduler<Shape>::empty()) {
            *ui_field_ = Field{};
            return;
        }

        *ui_field_ = Field{[](std::byte* buffer) {
            auto& header       = *new (buffer) Header{};
            header.sender_id   = msg::Id::RED_INFANTRY_III;
            header.receiver_id = msg::Id::RED_INFANTRY_III_CLIENT;
            size_t written     = sizeof(Header);

            int slot = 0;
            intptr_t updated[7];
            for (auto it = CfsScheduler<Shape>::get_update_iterator(); it && slot < 7;) {
                // Ignore text shape unless it is the first.
                if (it->is_text_shape()) {
                    if (slot == 0) {
                        header.command_id = 0x0110;
                        return written + it.update().write(buffer + written);
                    } else {
                        it.ignore();
                        continue;
                    }
                }

                auto operation = it->predict_update();
                if (operation == Shape::Operation::NO_OPERATION) {
                    it.ignore();
                    continue;
                }

                // Shapes are always aligned, so the last bits can be used to store information.
                auto identification =
                    reinterpret_cast<intptr_t>(it.get()) | (operation == Shape::Operation::ADD);
                // Ignore identical shapes that operate identically.
                if (std::find(updated, updated + slot, identification) != updated + slot) {
                    it.ignore();
                    continue;
                }

                written += it.update().write(buffer + written);

                updated[slot++] = identification;
            }

            constexpr std::pair<int, uint16_t> optional_packet[4] = {
                {1, 0x0101}, // draw 1 shape
                {2, 0x0102}, // draw 2 shapes
                {5, 0x0103}, // draw 5 shapes
                {7, 0x0104}, // draw 7 shapes
            };
            for (const auto& [shape_count, command_id] : optional_packet) {
                if (slot <= shape_count) {
                    for (; slot < shape_count; ++slot) {
                        written += Shape::no_operation_description().write(buffer + written);
                    }
                    header.command_id = command_id;
                    break;
                }
            }

            return written;
        }};
    }

private:
    OutputInterface<Field> ui_field_;
};

} // namespace rmcs_referee::command::interaction

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::command::interaction::Ui, rmcs_executor::Component)