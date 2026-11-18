#pragma once

#include <Wire.h>

#include "../i2c_bus.hpp"

class TwoWireI2cBus : public I2cBus {

    virtual BusStatus write(uint8_t dev_addr, const uint8_t* data, uint8_t len) override {
        if (!i2c.availableForWrite()) {
            return BusStatus::BufferFull;
        }

        return BusStatus::Ok;
    }

    virtual BusStatus read(uint8_t dev_addr, uint8_t* data, uint8_t len) override {

    }

    virtual BusStatus read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len) override {

    }

    virtual BusStatus write_reg(uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint8_t len) override {

    }

private:

    TwoWire& i2c;

};