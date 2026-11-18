#pragma once

#include <stdint.h>

enum class BusStatus : uint8_t {
    Ok = 0,
    Timeout,
    NACK_ADDR,
    NACK_DATA,
    BusError
};

class I2cBus {
public:

    virtual ~I2cBus() = default;

    I2cBus(const I2cBus&) = delete;
    I2cBus& operator=(const I2cBus&) = delete;

    virtual BusStatus write(uint8_t dev_addr, const uint8_t* data, size_t len) = 0;
    virtual BusStatus read(uint8_t dev_addr, uint8_t* data, size_t len) = 0;
    virtual BusStatus read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, size_t len) = 0;
    virtual BusStatus write_reg(uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, size_t len) = 0;

};
