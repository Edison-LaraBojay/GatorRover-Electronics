#pragma once

#include "device.hpp"

// Rover body frame (REP-103): x forward, y left, z up.
struct ImuSample {
    float accel_mps2[3] = {0, 0, 0};
    float gyro_radps[3] = {0, 0, 0};
    float temperature_c = 0;
    uint64_t timestamp_us = 0;
    bool valid = false;
};

class Imu : public Device {
public:
    explicit Imu(const char* name) : Device(name) {}

    const ImuSample& latest() const { return sample_; }

protected:
    ImuSample sample_;
};
