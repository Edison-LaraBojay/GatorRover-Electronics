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
        const Packet* packet = hw.jetson.peek_packet();

        if (!packet) {
            return;
        }

        hw.jetson.pop_packet();

        // extracting packet header
        uint8_t peripheral_id = packet->data[Packet::PERIPHERAL_ID_POS];
        uint8_t command = packet->data[Packet::COMMAND_POS];
        uint8_t sequence_id = packet->data[Packet::SEQUENCE_ID_POS];
        uint8_t payload_len = packet->data[Packet::LENGTH_POS];

        // extracting payload pointer
        const uint8_t* payload_received = &packet->data[Packet::PAYLOAD_START_POS];

        /// TODO: Write an example of transmitting data the the Jetson and an example of sending a command to a peripheral
        switch (command) {
            case IMU_SAMPLE: {

                uint8_t payload[IMU_SAMPLE_PAYLOAD_SIZE];

                /// TODO: Sample the IMU and populate the payload

                jetson.send_packet(peripheral_id, command, sequence_id, IMU_SAMPLE_PAYLOAD_SIZE, payload);

                break;
            }

            /// TODO: get the rest of the Teensy commands and implement them

            default:
                break;
        }
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
