#pragma once

#include "board_pins.hpp"

// Control board v1 (hardware/control-board/control-board-v1).
// PLACEHOLDER: every assignment below must be checked against the v1
// schematics before trusting it on hardware.
inline const Board& controlBoardV1() {
    static const Board board{
        "control-board-v1",
        /* jetson_serial     */ Serial1,   // RX1 = 0, TX1 = 1
        /* lidar_serial      */ Serial2,   // RX2 = 7, TX2 = 8
        /* i2c               */ Wire,      // SDA = 18, SCL = 19
        /* imu_gyro_int_pin  */ 2,
        /* imu_accel_int_pin */ 3,
        /* can_port          */ CanPort::Can1,
    };
    return board;
}
