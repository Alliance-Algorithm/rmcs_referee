#pragma once

#include <memory>

namespace referee {

class Collector final {
public:
    Collector();
    ~Collector();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

class Channel {
public:
    Channel();

private:
    uint8_t hz_ = 0;
};

} // namespace referee