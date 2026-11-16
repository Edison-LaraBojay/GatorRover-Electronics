#pragma once

// Which board, which transports, and which implementation of each device.
// The only file that names specific implementations.

#include "hardware_config.hpp"

#include "can_bus.hpp"
#include "connectors.hpp"
#include "control_board_v1.hpp"

#include "dummy/dummy_can_bus.hpp"
#include "dummy/dummy_imu.hpp"
#include "dummy/dummy_motor_controller.hpp"
#include "dummy/dummy_power_monitor.hpp"
#include "dummy/dummy_range_finder.hpp"

#include "jetson_link.hpp"
#include "command.hpp"


using DriveCanBus = DummyCanBus;
using ChassisImu  = DummyImu;
using FrontLidar  = DummyRangeFinder;
using MainPower   = DummyPowerMonitor;
using DriveMotor  = DummyMotorController;

struct Hardware {
    const Board& board = controlBoardV1();

    DriveCanBus drive_can {board.can_port, DriveCanBus::Config{}};

    void beginTransports() {
        board.i2c.begin();
        board.i2c.setClock(kI2cClockHz);
        board.jetson_serial.begin(kJetsonBaud);
        board.lidar_serial.begin(kLidarBaud);
        drive_can.begin();
    }

    void execute() {
        const Packet* request = hw.jetson.peek_packet();

        if (!request) {
            return;
        }

        switch (request->command) {
            case IMU_SAMPLE: {
                handle_imu_sample(request);
                break;
            }

            /// TODO: add more commands

            default: {
                break;
            }
        }

        jetson.pop_packet();
    }

    void handle_imu_sample(const Packet* request) {
        Packet response(
            request->peripheral_id, 
            request->command, 
            request->sequence_id, 
            IMU_SAMPLE_PAYLOAD_SIZE
        );

        size_t offset = 0;
        const ImuSample& sample = imu.latest();
        
        memcpy(response.payload + offset, sample.accel_mps2, sizeof(sample.accel_mps2));
        offset += sizeof(sample.accel_mps2);
        memcpy(response.payload + offset, sample.gyro_radps, sizeof(sample.gyro_radps));
        offset += sizeof(sample.gyro_radps);
        memcpy(response.payload + offset, &sample.temperature_c, sizeof(sample.temperature_c));

        jetson.send_packet(response);
    }

    // --- Devices: each reaches its transport through its connector --- //
    JetsonLink jetson {"jetson_link", {board.jetson_serial}};

    ChassisImu imu    {"imu",         {board.i2c, board.imu_gyro_int_pin, board.imu_accel_int_pin}, ChassisImu::Config{}};
    FrontLidar lidar  {"front_lidar", {board.lidar_serial}, FrontLidar::Config{}};
    MainPower  power  {"main_power",  {board.i2c},          MainPower::Config{}};

    DriveMotor drive_fl {"drive_fl", {drive_can, 1}, DriveMotor::Config{}};
    DriveMotor drive_fr {"drive_fr", {drive_can, 2}, DriveMotor::Config{}};
    DriveMotor drive_rl {"drive_rl", {drive_can, 3}, DriveMotor::Config{}};
    DriveMotor drive_rr {"drive_rr", {drive_can, 4}, DriveMotor::Config{}};

    MotorController* const drive_motors[4] = {&drive_fl, &drive_fr, &drive_rl, &drive_rr};
    Device* const devices[8] = {&jetson, &imu, &lidar, &power, &drive_fl, &drive_fr, &drive_rl, &drive_rr};
};
