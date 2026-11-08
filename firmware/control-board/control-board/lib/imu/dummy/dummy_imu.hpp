#pragma once

#include "connectors.hpp"
#include "imu.hpp"

struct DummyImuConfig {
    float temperature_c = 25.0f;
};

class DummyImu : public Imu {
public:
    using Config = DummyImuConfig;

    DummyImu(const char* name, const ImuConnector& /*connector*/, const Config& config)
        : Imu(name), config_(config) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void update(uint64_t now_us) override {
        sample_.accel_mps2[0] = 0.0f;
        sample_.accel_mps2[1] = 0.0f;
        sample_.accel_mps2[2] = 9.80665f;   // gravity, z up
        sample_.gyro_radps[0] = 0.0f;
        sample_.gyro_radps[1] = 0.0f;
        sample_.gyro_radps[2] = 0.0f;
        sample_.temperature_c = config_.temperature_c;
        sample_.timestamp_us = now_us;
        sample_.valid = true;
    }

private:
    const Config config_;
};
