#include "executor/collector.hpp"

namespace referee {

class Collector::Impl {
public:
    Impl() = default;
};

Collector::Collector()
    : pimpl(std::make_unique<Impl>())
{
}

Collector::~Collector() { }
} // namespace referee