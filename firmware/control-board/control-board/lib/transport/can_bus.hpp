#pragma once

#include <stdint.h>

struct CanFrame {
    uint32_t id = 0;
    bool extended = false;             // 29-bit id
    uint8_t len = 0;
    uint8_t data[8] = {0};
};

class CanBus {
public:
    virtual ~CanBus() = default;

    virtual bool begin() = 0;
    virtual bool send(const CanFrame& frame) = 0;
    virtual bool receive(CanFrame& frame) = 0;    // false if nothing waiting
};

// The Teensy 4.1's CAN peripherals, the CAN counterpart of Serial2 or Wire.
enum class CanPort : uint8_t {
    Can1,   // TX 22, RX 23
    Can2,   // TX 1,  RX 0
    Can3,   // TX 31, RX 30
};

// Connector for a device on a CAN bus: the shared bus plus its id on it.
struct CanDeviceConnector {
    CanBus& bus;
    uint8_t id;
};
