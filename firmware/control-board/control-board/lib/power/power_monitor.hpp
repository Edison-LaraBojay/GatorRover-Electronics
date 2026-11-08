#pragma once

#include "device.hpp"

struct PowerSample {
    float bus_voltage_v = 0;
    float current_a = 0;               
    float power_w = 0;
    double energy_wh = 0;
    uint64_t timestamp_us = 0;
    bool valid = false;
};

class PowerMonitor : public Device {
public:
    explicit PowerMonitor(const char* name) : Device(name) {}

    const PowerSample& latest() const { return sample_; }

    void resetEnergy() { sample_.energy_wh = 0; }

protected:
    PowerSample sample_;
};
