#pragma once

// Which board, which transports, and which implementation of each device.
// The only file that names specific implementations.

#include "can_bus.hpp"
#include "connectors.hpp"
#include "control_board_v1.hpp"

#include "dummy/dummy_can_bus.hpp"
#include "dummy/dummy_imu.hpp"
#include "dummy/dummy_motor_controller.hpp"
#include "dummy/dummy_power_monitor.hpp"
#include "dummy/dummy_range_finder.hpp"

using DriveCanBus = DummyCanBus;
using ChassisImu  = DummyImu;
using FrontLidar  = DummyRangeFinder;
using MainPower   = DummyPowerMonitor;
using DriveMotor  = DummyMotorController;

struct Hardware {
    const Board& board = controlBoardV1();

    static constexpr uint32_t kI2cClockHz = 400000;
    static constexpr uint32_t kJetsonBaud = 115200;
    static constexpr uint32_t kLidarBaud  = 115200;

    DriveCanBus drive_can {board.can_port, DriveCanBus::Config{}};

    void beginTransports() {
        board.i2c.begin();
        board.i2c.setClock(kI2cClockHz);
        board.jetson_serial.begin(kJetsonBaud);
        board.lidar_serial.begin(kLidarBaud);
        drive_can.begin();
    }

    // ---- Devices: each reaches its transport through its connector -------
    ChassisImu imu   {"imu",         {board.i2c, board.imu_gyro_int_pin, board.imu_accel_int_pin}, ChassisImu::Config{}};
    FrontLidar lidar {"front_lidar", {board.lidar_serial}, FrontLidar::Config{}};
    MainPower  power {"main_power",  {board.i2c},          MainPower::Config{}};

    DriveMotor drive_fl {"drive_fl", {drive_can, 1}, DriveMotor::Config{}};
    DriveMotor drive_fr {"drive_fr", {drive_can, 2}, DriveMotor::Config{}};
    DriveMotor drive_rl {"drive_rl", {drive_can, 3}, DriveMotor::Config{}};
    DriveMotor drive_rr {"drive_rr", {drive_can, 4}, DriveMotor::Config{}};

    MotorController* const drive_motors[4] = {&drive_fl, &drive_fr, &drive_rl, &drive_rr};
    Device* const devices[7] = {&imu, &lidar, &power, &drive_fl, &drive_fr, &drive_rl, &drive_rr};
};
