#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// What each device implementation receives: the bus or port behind its
// connector plus any extra pins. Built in src/hardware.hpp.
// CAN devices use CanDeviceConnector (can_bus.hpp).

struct JetsonConnector {
    HardwareSerial& serial;
};

// 6-pin JST-PH: 3V3, SDA, SCL, GYRO_INT, ACCEL_INT, GND.
struct ImuConnector {
    TwoWire& i2c;
    uint8_t gyro_int_pin;
    uint8_t accel_int_pin;
};

struct LidarConnector {
    HardwareSerial& serial;
};

// 4-pin JST: 3V3, GND, SDA, SCL.
struct PowerLoggerConnector {
    TwoWire& i2c;
};
