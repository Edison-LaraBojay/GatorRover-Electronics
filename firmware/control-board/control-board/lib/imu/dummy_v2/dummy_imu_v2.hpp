#pragma once

#include "connectors.hpp"
#include "imu.hpp"

struct DummyImuV2Config {
    float temperature_c = 25.0f;
    float accel_scale = 0.000061f; // e.g., +/- 2g sensitivity (g/LSB)
    float gyro_scale = 0.00875f;   // e.g., +/- 250 dps sensitivity (dps/LSB)
};

/**
 * Used to outline use of the I2cBus wrapper and how to read IMU values.
 */
class DummyImuV2 : public Imu {
public:
    using Config = DummyImuV2Config;

    DummyImuV2(const char* name, const ImuConnector& connector, const Config& config)
        : Imu(name), connector_(connector), config_(config) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void update(uint64_t now_us) override {
        uint8_t raw_gyro[6] = {0};
        uint8_t raw_accel[6] = {0};

        // Burst-read 6 bytes starting from the lowest register of each block.
        // Assumes that the device read pointers automatically increment with each read. Many real devices have this feature.
        BusStatus gyro_ok = connector_.i2c_bus.read_reg(GYRO_I2C_ADDR, GYRO_OUT_X_L, raw_gyro, sizeof(raw_gyro));
        BusStatus accel_ok = connector_.i2c_bus.read_reg(ACCEL_I2C_ADDR, ACCEL_OUT_X_L, raw_accel, sizeof(raw_accel));

        if (gyro_ok != BusStatus::Ok || accel_ok != BusStatus::Ok) {
            sample_.valid = false;
            return;
        }

        float gyro_vals[3];
        float accel_vals[3];

        for (int i = 0; i < 3; ++i) {
            int16_t raw_g = static_cast<int16_t>((raw_gyro[i * 2 + 1] << 8) | raw_gyro[i * 2]);
            int16_t raw_a = static_cast<int16_t>((raw_accel[i * 2 + 1] << 8) | raw_accel[i * 2]);

            gyro_vals[i] = static_cast<float>(raw_g) * config_.gyro_scale;
            accel_vals[i] = static_cast<float>(raw_a) * config_.accel_scale;
        }

        memcpy(sample_.gyro_radps, gyro_vals, sizeof(sample_.gyro_radps));
        memcpy(sample_.accel_mps2, accel_vals, sizeof(sample_.accel_mps2));

        sample_.temperature_c = config_.temperature_c;
        sample_.timestamp_us = now_us;
        sample_.valid = true;
    }

private:

    // --- Arbitrary values --- //

    static constexpr uint8_t ACCEL_I2C_ADDR = 0x18;
    static constexpr uint8_t GYRO_I2C_ADDR  = 0x6A;


    // 16-bit registers

    static constexpr uint8_t GYRO_OUT_X_L = 0x22;
    static constexpr uint8_t GYRO_OUT_X_H = 0x23;
    static constexpr uint8_t GYRO_OUT_Y_L = 0x24;
    static constexpr uint8_t GYRO_OUT_Y_H = 0x25;
    static constexpr uint8_t GYRO_OUT_Z_L = 0x26;
    static constexpr uint8_t GYRO_OUT_Z_H = 0x27;

    static constexpr uint8_t ACCEL_OUT_X_L = 0x28;
    static constexpr uint8_t ACCEL_OUT_X_H = 0x29;
    static constexpr uint8_t ACCEL_OUT_Y_L = 0x2A;
    static constexpr uint8_t ACCEL_OUT_Y_H = 0x2B;
    static constexpr uint8_t ACCEL_OUT_Z_L = 0x2C;
    static constexpr uint8_t ACCEL_OUT_Z_H = 0x2D;


    const Config config_;
    ImuConnector connector_;
};
