#pragma once

#include <stdint.h>

#include "can_bus.hpp"

struct DummyCanBusConfig {
    uint32_t bitrate = 1000000;
};

// Drops every frame sent, never receives any.
class DummyCanBus : public CanBus {
public:
    using Config = DummyCanBusConfig;

    DummyCanBus(CanPort /*port*/, const Config& /*config*/) {}

    bool begin() override { return true; }
    bool send(const CanFrame&) override { return true; }
    bool receive(CanFrame&) override { return false; }
};
