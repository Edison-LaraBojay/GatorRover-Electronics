#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "can_bus.hpp"

// What a control board physically has: which Teensy peripheral and pins
// each connector uses. Nothing here is started; beginTransports() in
// src/hardware.hpp starts them.
//
// Teensy 4.1 pins that collide (only one user each):
//   Serial1 (0, 1)   <-> CAN2 (0, 1)
//   Serial4 (16, 17) <-> Wire1 (16, 17)
//   Serial6 (24, 25) <-> Wire2 (24, 25)
//
// Each board revision fills this in its own file (control_board_v1.hpp, ...).
struct Board {
    const char* name;

    HardwareSerial& jetson_serial;     // 40-pin ribbon to the Jetson
    HardwareSerial& lidar_serial;      // LiDAR JST: 5V, TX, RX, GND

    TwoWire& i2c;                      // IMU and PowerLogger JSTs share this bus
    uint8_t imu_gyro_int_pin;
    uint8_t imu_accel_int_pin;

    CanPort can_port;
};
