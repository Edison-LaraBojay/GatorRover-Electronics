#pragma once

#include <stdint.h>
#include <stddef.h>

#include "packet.hpp"


// --- Commands --- //

static constexpr uint8_t IMU_SAMPLE = 1;
static constexpr uint8_t IMU_ACCEL = IMU_SAMPLE + 1;
static constexpr uint8_t IMU_GYRO = IMU_ACCEL + 1;
static constexpr uint8_t IMU_TEMP = IMU_GYRO + 1;

static constexpr uint8_t MOTOR_CTRL_SET_DUTY = IMU_TEMP + 1;
static constexpr uint8_t MOTOR_CTRL_SET_VELOCITY = MOTOR_CTRL_SET_DUTY + 1;
static constexpr uint8_t MOTOR_CTRL_DISABLE = MOTOR_CTRL_SET_VELOCITY + 1;
static constexpr uint8_t MOTOR_CTRL_CAPABILITIES = MOTOR_CTRL_DISABLE + 1;
static constexpr uint8_t MOTOR_CTRL_FEEDBACK = MOTOR_CTRL_CAPABILITIES + 1;
static constexpr uint8_t MOTOR_CTRL_VELOCITY = MOTOR_CTRL_FEEDBACK + 1;
static constexpr uint8_t MOTOR_CTRL_POSITION = MOTOR_CTRL_VELOCITY + 1;
static constexpr uint8_t MOTOR_CTRL_CURRENT = MOTOR_CTRL_POSITION + 1;
static constexpr uint8_t MOTOR_CTRL_BUS_VOLTAGE = MOTOR_CTRL_CURRENT + 1;
static constexpr uint8_t MOTOR_CTRL_TEMP = MOTOR_CTRL_BUS_VOLTAGE + 1;
static constexpr uint8_t MOTOR_CTRL_FAULT = MOTOR_CTRL_TEMP + 1;

static constexpr uint8_t RANGE_SAMPLE = MOTOR_CTRL_FAULT + 1;
static constexpr uint8_t RANGE_DISTANCE = RANGE_SAMPLE + 1;
static constexpr uint8_t RANGE_SIGNAL_STRENGTH = RANGE_DISTANCE + 1;
static constexpr uint8_t RANGE_TEMP = RANGE_SIGNAL_STRENGTH + 1;
static constexpr uint8_t RANGE_MIN = RANGE_TEMP + 1;
static constexpr uint8_t RANGE_MAX = RANGE_MIN + 1;

static constexpr uint8_t POWER_SAMPLE = RANGE_MAX + 1;
static constexpr uint8_t POWER_VOLTAGE = POWER_SAMPLE + 1;
static constexpr uint8_t POWER_CURRENT = POWER_VOLTAGE + 1;
static constexpr uint8_t POWER_WATTS = POWER_CURRENT + 1;
static constexpr uint8_t POWER_ENERGY = POWER_WATTS + 1;
static constexpr uint8_t POWER_RESET_ENERGY = POWER_ENERGY + 1;



// --- Payload Sizes --- //
// Sizes describe the serialized measurement values only. 
// Timestamps, valid flags, and optional-field presence flags are
// excluded from the payloads unless explicitly stated below.

// IMU: accel[3], gyro[3], temperature

static constexpr size_t IMU_SAMPLE_PAYLOAD_SIZE = sizeof(float) * 7;
static constexpr size_t IMU_ACCEL_PAYLOAD_SIZE = sizeof(float) * 3;
static constexpr size_t IMU_GYRO_PAYLOAD_SIZE = sizeof(float) * 3;
static constexpr size_t IMU_TEMP_PAYLOAD_SIZE = sizeof(float);


// Motor control requests

static constexpr size_t MOTOR_CTRL_SET_DUTY_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_SET_VELOCITY_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_DISABLE_PAYLOAD_SIZE = 0;


// Motor responses: two capability flags; feedback is a compact fixed layout
// containing 5 presence flags, 5 floats, fault, timestamp, and valid.

static constexpr size_t MOTOR_CTRL_CAPABILITIES_PAYLOAD_SIZE = 2;
static constexpr size_t MOTOR_CTRL_FEEDBACK_PAYLOAD_SIZE = 5 + sizeof(float) * 5 + 1 + sizeof(uint64_t) + 1;
static constexpr size_t MOTOR_CTRL_VELOCITY_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_POSITION_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_CURRENT_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_BUS_VOLTAGE_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_TEMP_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t MOTOR_CTRL_FAULT_PAYLOAD_SIZE = 1;


// Rangefinder: distance, signal strength, temperature

static constexpr size_t RANGE_SAMPLE_PAYLOAD_SIZE = sizeof(float) * 2 + sizeof(uint16_t);
static constexpr size_t RANGE_DISTANCE_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t RANGE_SIGNAL_STRENGTH_PAYLOAD_SIZE = sizeof(uint16_t);
static constexpr size_t RANGE_TEMP_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t RANGE_MIN_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t RANGE_MAX_PAYLOAD_SIZE = sizeof(float);


// Power monitor: bus voltage, current, power, energy

static constexpr size_t POWER_SAMPLE_PAYLOAD_SIZE = sizeof(float) * 3 + sizeof(double);
static constexpr size_t POWER_VOLTAGE_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t POWER_CURRENT_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t POWER_WATTS_PAYLOAD_SIZE = sizeof(float);
static constexpr size_t POWER_ENERGY_PAYLOAD_SIZE = sizeof(double);
static constexpr size_t POWER_RESET_ENERGY_PAYLOAD_SIZE = 0;
