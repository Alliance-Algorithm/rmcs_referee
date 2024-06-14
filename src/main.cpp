#include "send/interact/packer.hpp"
#include "send/interact/shape.hpp"

#include <serial/serial.h>
#include <thread>

int main()
{
    using namespace referee;
    using namespace std::chrono_literals;

    auto packer = referee::executor::Packer();
    packer.set_id(3);
    packer.set_client_id(0x0103);

    auto line = ui::Line {};
    line.set_name("000");
    line.color        = ui::ColorEnum::GREEN;
    line.operation    = ui::OperationEnum::ADD;
    line.width        = 10;
    line.x            = 600;
    line.layer        = 1;
    line.y            = 350;
    line.x_end        = 600 + 200;
    line.y_end        = 350;
    auto line_package = packer.pack_shape(line.generate_description());

    auto string = ui::String {};
    string.set_name("222");
    string.color        = ui::ColorEnum::GREEN;
    string.operation    = ui::OperationEnum::ADD;
    string.layer        = 1;
    string.font_size    = 20;
    string.length       = 2;
    string.width        = 2;
    string.x            = 600 + 5;
    string.y            = 500;
    auto string_package = packer.pack_string(string.generate_description(), "Genshin Impact");

    auto integer = ui::Integer {};
    integer.set_name("234");
    integer.color        = ui::ColorEnum::GREEN;
    integer.operation    = ui::OperationEnum::MODIFY;
    integer.layer        = 1;
    integer.font_size    = 10;
    integer.width        = 1;
    integer.x            = 600;
    integer.y            = 800;
    integer.value        = 123;
    auto integer_package = packer.pack_shape(integer.generate_description());

    auto circle_add = ui::Circle {};
    circle_add.set_name("123");
    circle_add.color     = ui::ColorEnum::SELF;
    circle_add.operation = ui::OperationEnum::ADD;
    circle_add.radius    = 100;
    circle_add.layer     = 1;
    circle_add.x         = 960;
    circle_add.y         = 540;
    circle_add.width     = 10;

    auto circle = ui::Circle {};
    circle.set_name("292");
    circle.color     = ui::ColorEnum::SELF;
    circle.operation = ui::OperationEnum::ADD;
    circle.radius    = 100;
    circle.layer     = 1;
    circle.x         = 960;
    circle.y         = 540;
    circle.width     = 10;

    auto delete_package = packer.pack_delete_layer();

    auto package_7 = packer.pack_shape(
        line.generate_description(),
        integer.generate_description(),
        integer.generate_description(),
        integer.generate_description(),
        integer.generate_description(),
        integer.generate_description(),
        integer.generate_description());

    auto package_2 = packer.pack_shape(line.generate_description(), line.generate_description());

    auto serial = serial::Serial("/dev/ttyUSB0", 115200);

    serial.write(reinterpret_cast<uint8_t*>(&delete_package), sizeof(delete_package));
    std::this_thread::sleep_for(100ms);

    while (true) {
        serial.write(reinterpret_cast<uint8_t*>(&delete_package), sizeof(delete_package));
        std::this_thread::sleep_for(100ms);

        integer.operation = referee::ui::OperationEnum::ADD;
        for (int i = 0; i < 50; i++) {

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 650;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 0;
            auto i_0        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 680;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 1;
            auto i_1        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 710;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 2;
            auto i_2        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 740;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 3;
            auto i_3        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 770;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 4;
            auto i_4        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 800;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 5;
            auto i_5        = integer.generate_description();

            integer.value   = i;
            integer.x       = i * 30;
            integer.y       = 830;
            integer.name[0] = i;
            integer.name[1] = i;
            integer.name[2] = i;
            integer.layer   = 6;
            auto i_6        = integer.generate_description();

            auto package = packer.pack_shape(i_0, i_1, i_2, i_3, i_4);

            serial.write(reinterpret_cast<uint8_t*>(&package), sizeof(package));
            std::this_thread::sleep_for(40ms);
        }

        std::this_thread::sleep_for(1000ms);
    }

    return 0;
}