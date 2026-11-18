#pragma once

#include <Wire.h>

#include "../i2c_bus.hpp"

class DummyI2cBus : public I2cBus {

    virtual BusStatus write(uint8_t dev_addr, const uint8_t* data, size_t len) {

    }

    virtual BusStatus read(uint8_t dev_addr, uint8_t* data, size_t len) {

    }

    virtual BusStatus read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, size_t len) {

    }

    virtual BusStatus write_reg(uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, size_t len) {

    }

private:

    TwoWire& i2c;

};