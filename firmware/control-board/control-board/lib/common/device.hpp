#pragma once

#include <stdint.h>

// Device health. Whether a single reading is usable is the sample's `valid`.
enum class DeviceStatus : uint8_t {
    NotStarted = 0,
    Ok,
    NotDetected,
    Timeout,
    BadData,
    Fault,
};

inline const char* toString(DeviceStatus s) {
    switch (s) {
        case DeviceStatus::NotStarted:  return "NOT_STARTED";
        case DeviceStatus::Ok:          return "OK";
        case DeviceStatus::NotDetected: return "NOT_DETECTED";
        case DeviceStatus::Timeout:     return "TIMEOUT";
        case DeviceStatus::BadData:     return "BAD_DATA";
        case DeviceStatus::Fault:       return "FAULT";
    }
    return "?";
}

class Device {
public:
    explicit Device(const char* name) : name_(name) {}
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    const char* name() const { return name_; }

    virtual bool begin() = 0;
    virtual void update(uint64_t now_us) = 0;     // must not block

    DeviceStatus status() const { return status_; }

protected:
    DeviceStatus status_ = DeviceStatus::NotStarted;

private:
    const char* const name_;
};
