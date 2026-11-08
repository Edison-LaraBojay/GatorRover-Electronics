#pragma once

#include "connectors.hpp"
#include "power_monitor.hpp"

struct DummyPowerMonitorConfig {
    float voltage_v = 24.0f;
    float current_a = 1.5f;
};

// Constant voltage and current; energy accumulates.
class DummyPowerMonitor : public PowerMonitor {
public:
    using Config = DummyPowerMonitorConfig;

    DummyPowerMonitor(const char* name, const PowerLoggerConnector& /*connector*/, const Config& config)
        : PowerMonitor(name), config_(config) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void update(uint64_t now_us) override {
        if (sample_.valid) {
            const double dt_h = (now_us - sample_.timestamp_us) / 3.6e9;
            sample_.energy_wh += sample_.power_w * dt_h;
        }
        sample_.bus_voltage_v = config_.voltage_v;
        sample_.current_a = config_.current_a;
        sample_.power_w = config_.voltage_v * config_.current_a;
        sample_.timestamp_us = now_us;
        sample_.valid = true;
    }

private:
    const Config config_;
};
