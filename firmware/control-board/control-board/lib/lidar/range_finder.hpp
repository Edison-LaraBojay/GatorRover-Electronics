#pragma once

#include "device.hpp"

// Single-point LiDAR / rangefinder.
struct RangeSample {
    float distance_m = 0;
    uint16_t signal_strength = 0;      // sensor-specific units
    float temperature_c = 0;
    uint64_t timestamp_us = 0;
    bool valid = false;                // false = no reliable target
};

class RangeFinder : public Device {
public:
    explicit RangeFinder(const char* name) : Device(name) {}

    const RangeSample& latest() const { return sample_; }

    virtual float minRangeM() const = 0;
    virtual float maxRangeM() const = 0;

protected:
    RangeSample sample_;
};
