#pragma once

#include <stdint.h>


// Uses Arduino's I2C status codes  
enum class BusStatus : uint8_t {
    Ok        = 0,
    NACK_ADDR = 2,
    NACK_DATA = 3,
    Timeout   = 5,
    BusError,
    BufferFull,
    InvalidReq
};

class I2cBus {
public:

    virtual ~I2cBus() = default;

    virtual BusStatus write(uint8_t dev_addr, const uint8_t* data, uint8_t len) = 0;
    virtual BusStatus read(uint8_t dev_addr, uint8_t* data, uint8_t len) = 0;
    virtual BusStatus read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint8_t len) = 0;
    virtual BusStatus write_reg(uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint8_t len) = 0;

};
