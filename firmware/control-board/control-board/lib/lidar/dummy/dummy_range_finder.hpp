#pragma once

#include "connectors.hpp"
#include "range_finder.hpp"

struct DummyRangeFinderConfig {
    float distance_m = 2.0f;
};

// Target at a fixed distance.
class DummyRangeFinder : public RangeFinder {
public:
    using Config = DummyRangeFinderConfig;

    DummyRangeFinder(const char* name, const LidarConnector& /*connector*/, const Config& config)
        : RangeFinder(name), config_(config) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void update(uint64_t now_us) override {
        sample_.distance_m = config_.distance_m;
        sample_.signal_strength = 1000;
        sample_.temperature_c = 25.0f;
        sample_.timestamp_us = now_us;
        sample_.valid = config_.distance_m >= minRangeM() && config_.distance_m <= maxRangeM();
    }

    float minRangeM() const override { return 0.1f; }
    float maxRangeM() const override { return 40.0f; }

private:
    const Config config_;
};
