#pragma once

#include "device.hpp"

struct MotorCapabilities {
    bool velocity_control = false;     // closed-loop velocity on the controller
    bool feedback = false;
};

// has_* = this controller reports that value.
struct MotorFeedback {
    bool has_velocity = false;
    float velocity_rpm = 0;            // motor shaft

    bool has_position = false;
    float position_rot = 0;            // motor shaft

    bool has_current = false;
    float current_a = 0;

    bool has_bus_voltage = false;
    float bus_voltage_v = 0;

    bool has_temperature = false;
    float temperature_c = 0;

    bool fault = false;
    uint64_t timestamp_us = 0;
    bool valid = false;
};

class MotorController : public Device {
public:
    explicit MotorController(const char* name) : Device(name) {}

    virtual MotorCapabilities capabilities() const = 0;

    virtual void setDutyCycle(float duty) = 0;    // [-1, 1]
    virtual bool setVelocityRpm(float rpm) = 0;   // false if not supported
    virtual void disable() = 0;

    const MotorFeedback& feedback() const { return feedback_; }

protected:
    MotorFeedback feedback_;
};
