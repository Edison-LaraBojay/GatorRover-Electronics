#pragma once

#include "can_bus.hpp"
#include "motor_controller.hpp"

struct DummyMotorControllerConfig {
    float free_speed_rpm = 5676.0f;    // speed at full duty cycle
};

// Ideal motor that instantly reaches the commanded speed. Ignores the connector.
class DummyMotorController : public MotorController {
public:
    using Config = DummyMotorControllerConfig;

    DummyMotorController(const char* name, const CanDeviceConnector& /*connector*/, const Config& config)
        : MotorController(name), config_(config) {}

    MotorCapabilities capabilities() const override { return {true, true}; }

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void setDutyCycle(float duty) override {
        target_rpm_ = clamp(duty, 1.0f) * config_.free_speed_rpm;
    }

    bool setVelocityRpm(float rpm) override {
        target_rpm_ = clamp(rpm, config_.free_speed_rpm);
        return true;
    }

    void disable() override { target_rpm_ = 0.0f; }

    void update(uint64_t now_us) override {
        if (feedback_.valid) {
            const float dt_s = (now_us - feedback_.timestamp_us) / 1e6f;
            feedback_.position_rot += target_rpm_ / 60.0f * dt_s;
        }
        feedback_.has_velocity = true;
        feedback_.velocity_rpm = target_rpm_;
        feedback_.has_position = true;
        feedback_.has_bus_voltage = true;
        feedback_.bus_voltage_v = 12.0f;
        feedback_.timestamp_us = now_us;
        feedback_.valid = true;
    }

private:
    static float clamp(float v, float limit) {
        return v > limit ? limit : (v < -limit ? -limit : v);
    }

    const Config config_;
    float target_rpm_ = 0.0f;
};
