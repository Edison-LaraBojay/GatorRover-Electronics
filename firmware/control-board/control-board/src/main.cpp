#include <Arduino.h>

#include "clock.hpp"
#include "hardware.hpp"

namespace {
Hardware hw;
}

void setup() {
    hw.beginTransports();
    for (Device* d : hw.devices) {
        d->begin();
    }
}

void loop() {
    const uint64_t now = sysclock::nowUs();
    for (Device* d : hw.devices) {
        d->update(now);
    }
}
