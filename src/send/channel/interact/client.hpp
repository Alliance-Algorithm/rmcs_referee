#pragma once

#include <memory>

namespace referee {
class Updater {
public:
    Updater(const std::string& port, uint32_t baudrate);
    ~Updater();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

inline auto updater = Updater { "", 115200 };
} // namespace referee