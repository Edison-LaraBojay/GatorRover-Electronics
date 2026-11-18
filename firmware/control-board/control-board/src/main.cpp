#include <Arduino.h>

#include "clock.hpp"
#include "hardware.hpp"
#include "controller.hpp"

namespace {
Hardware hw;
RoverController controller(hw);
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

    controller.execute();
}
