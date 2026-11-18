#pragma once

#include <Arduino.h>
#include <Wire.h>

struct I2cBus {

    TwoWire& i2c;
}