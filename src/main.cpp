#include "send/channel/interact/basic.hpp"

int main()
{
    using namespace referee;

    auto referee = BasicInterface("/dev/ttyUSB0", 115200);
    referee.set_id(3);
    referee.set_client_id(0x0103);

    auto a = ui::Line {};
    a.set_name("111");
    a.color     = ui::ColorEnum::GREEN;
    a.operation = ui::OperationEnum::DELETE;
    a.width     = 10;
    a.x         = 600;
    a.y         = 350;
    a.end_x     = 600 + 200;
    a.end_y     = 350;

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

    auto i = ui::Integer {};
    i.set_name("444");
    i.color     = ui::ColorEnum::GREEN;
    i.operation = ui::OperationEnum::MODIFY;
    i.layer     = 1;
    i.font_size = 20;
    i.width     = 2;
    i.x         = 600;
    i.y         = 360;
    i.value     = 150;

    // referee.delete_layer();
    // referee.apply_description(i.generate_description());
    referee.draw_string(string.generate_description(), std::string("HPS"));

    return 0;
}