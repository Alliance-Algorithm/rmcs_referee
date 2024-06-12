#include "send/channel/interact/package.hpp"

int main()
{
    using namespace referee;

    auto packer = Packer();
    packer.set_id(3);
    packer.set_client_id(0x0103);

    auto line = ui::Line {};
    line.set_name("111");
    line.color     = ui::ColorEnum::GREEN;
    line.operation = ui::OperationEnum::DELETE;
    line.width     = 10;
    line.x         = 600;
    line.y         = 350;
    line.end_x     = 600 + 200;
    line.end_y     = 350;

    auto string = ui::String {};
    string.set_name("222");
    string.color     = ui::ColorEnum::GREEN;
    string.operation = ui::OperationEnum::MODIFY;
    string.layer     = 1;
    string.font_size = 20;
    string.length    = 2;
    string.width     = 2;
    string.x         = 600 + 5;
    string.y         = 360;

    auto integer = ui::Integer {};
    integer.set_name("444");
    integer.color     = ui::ColorEnum::GREEN;
    integer.operation = ui::OperationEnum::MODIFY;
    integer.layer     = 1;
    integer.font_size = 20;
    integer.width     = 2;
    integer.x         = 600;
    integer.y         = 360;
    integer.value     = 150;

    auto package = packer.pack_shapes(
        std::array<ui::Description, 5> {
            integer.generate_description(),
            integer.generate_description(),
            integer.generate_description(),
            integer.generate_description(),
            integer.generate_description() });

    return 0;
}