#pragma once

#include "protocol/dji_crc.hpp"
#include "send/interact/package.hpp"

#include <array>

namespace referee::executor {
class Packer {
public:
    Packer() = default;

    ui::StringPackage pack_string(const ui::Description& description, const std::string& string)
    {
        auto package                  = ui::StringPackage();
        package.header.length         = sizeof(package.data);
        package.header.sequence       = sequence_++;
        package.command               = 0x0301;
        package.data.command          = 0x0110;
        package.data.sender           = sender_;
        package.data.receiver         = client_;
        package.data.data.description = description;
        string.copy(reinterpret_cast<char*>(package.data.data.data), string.length());

        serial_util::dji_crc::append_crc8(package.header);
        serial_util::dji_crc::append_crc16(package);

        return package;
    }

    ui::DeletePackage pack_delete_layer(int index)
    {
        auto package                  = ui::DeletePackage();
        package.header.length         = sizeof(package.data);
        package.header.sequence       = sequence_++;
        package.command               = 0x0301;
        package.data.command          = 0x0100;
        package.data.sender           = sender_;
        package.data.receiver         = client_;
        package.data.data.delete_type = 1;
        package.data.data.layer       = index;

        serial_util::dji_crc::append_crc8(package.header);
        serial_util::dji_crc::append_crc16(package);

        return package;
    }

    ui::DeletePackage pack_delete_layer()
    {
        auto package                  = ui::DeletePackage();
        package.header.length         = sizeof(package.data);
        package.header.sequence       = sequence_++;
        package.command               = 0x0301;
        package.data.command          = 0x0100;
        package.data.sender           = sender_;
        package.data.receiver         = client_;
        package.data.data.delete_type = 2;

        serial_util::dji_crc::append_crc8(package.header);
        serial_util::dji_crc::append_crc16(package);

        return package;
    }

    ui::DrawPackage1 pack_shape(const ui::Description& description)
    {
        auto package            = ui::DrawPackage1();
        package.header.length   = sizeof(package.data);
        package.header.sequence = sequence_++;
        package.command         = 0x0301;
        package.data.command    = 0x0101;
        package.data.sender     = sender_;
        package.data.receiver   = client_;
        package.data.data       = description;

        serial_util::dji_crc::append_crc8(package.header);
        serial_util::dji_crc::append_crc16(package);

        return package;
    }

    template <size_t n>
    requires(n == 2 || n == 5 || n == 7)
    decltype(auto) pack_shapes(const std::array<ui::Description, n>& description)
    {
        if constexpr (n == 2) {
            auto package            = ui::DrawPackage2();
            package.header.length   = sizeof(package.data);
            package.header.sequence = sequence_++;
            package.command         = 0x0301;
            package.data.command    = 0x0102;
            package.data.sender     = sender_;
            package.data.receiver   = client_;

            for (int i = 0; i < n; i++) {
                package.data.data.description[i] = description[i];
            }

            serial_util::dji_crc::append_crc8(package.header);
            serial_util::dji_crc::append_crc16(package);

            return package;

        } else if constexpr (n == 5) {
            auto package            = ui::DrawPackage5();
            package.header.length   = sizeof(package.data);
            package.header.sequence = sequence_++;
            package.command         = 0x0301;
            package.data.command    = 0x0103;
            package.data.sender     = sender_;
            package.data.receiver   = client_;

            for (int i = 0; i < n; i++) {
                package.data.data.description[i] = description[i];
            }

            serial_util::dji_crc::append_crc8(package.header);
            serial_util::dji_crc::append_crc16(package);

            return package;

        } else if constexpr (n == 7) {
            auto package            = ui::DrawPackage7();
            package.header.length   = sizeof(package.data);
            package.header.sequence = sequence_++;
            package.command         = 0x0301;
            package.data.command    = 0x0104;
            package.data.sender     = sender_;
            package.data.receiver   = client_;

            for (int i = 0; i < n; i++) {
                package.data.data.description[i] = description[i];
            }

            serial_util::dji_crc::append_crc8(package.header);
            serial_util::dji_crc::append_crc16(package);

            return package;
        }
    }

    // config
    void set_id(uint16_t id)
    {
        sender_ = id;
    }

    void set_client_id(uint16_t id)
    {
        client_ = id;
    }

private:
    // for test
    static void basic_information(ui::DrawPackage1& package)
    {
        std::printf("\n");
        std::printf("%-20s%-20zu\n", "size ", sizeof(package));
        std::printf("%-20s%-#20x\n", "header.start ", package.header.start);
        std::printf("%-20s%-20d\n", "header.length ", package.header.length);
        std::printf("%-20s%-20d\n", "header.sequence ", package.header.sequence);
        std::printf("%-20s%-20d\n", "header.crc8 ", package.header.crc8);
        std::printf("%-20s%-#20x\n", "command ", package.command);
        std::printf("%-20s%-20d\n", "crc16 ", package.crc16);
        std::printf("%-20s%-#20x\n", "data.command ", package.data.command);
        std::printf("%-20s%-#20x\n", "data.sender ", package.data.sender);
        std::printf("%-20s%-#20x\n", "data.receiver ", package.data.receiver);
        std::printf("%-20s%-20.3s\n", "data.data.name ", package.data.data.name);
        std::printf("%-20s%-20d\n", "data.data.operate ", package.data.data.operation);
        std::printf("%-20s%-20d\n", "data.data.graphic ", package.data.data.shape);
        std::printf("%-20s%-20d\n", "data.data.layer ", package.data.data.layer);
        std::printf("%-20s%-20d\n", "data.data.color ", package.data.data.color);
        std::printf("%-20s%-20d\n", "data.data.width ", package.data.data.width);
        std::printf("%-20s%-20d\n", "data.data.start_x ", package.data.data.start_x);
        std::printf("%-20s%-20d\n", "data.data.start_y ", package.data.data.start_y);
    }

private:
    uint16_t sender_  = 0;
    uint16_t client_  = 0;
    uint8_t sequence_ = 0;
};
} // namespace referee::executor