#include "send/channel/interact/client.hpp"
#include "send/channel/interact/basic.hpp"

namespace referee {
class Updater::Impl : public BasicInterface {
public:
    Impl(const std::string& port, uint32_t baudrate)
        : BasicInterface(port, baudrate) {};
};

Updater::Updater(const std::string& port, uint32_t baudrate)
    : pimpl(std::make_unique<Impl>(port, baudrate))
{
}

Updater::~Updater() { }
} // namespace referee