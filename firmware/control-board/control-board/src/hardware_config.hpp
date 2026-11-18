#include "packet.hpp"

#include <Arduino.h>


// --- Transport Frequencies --- //

static constexpr uint32_t kI2cClockHz = 400000;
static constexpr uint32_t kJetsonBaud = 115200;
static constexpr uint32_t kLidarBaud  = 115200;


// --- Device/Peripheral IDs --- //

static constexpr size_t JETSONLINK_ID   = 0;
static constexpr size_t IMU_ID      = JETSONLINK_ID + 1;
static constexpr size_t LIDAR_ID    = IMU_ID + 1;
static constexpr size_t POWER_ID    = LIDAR_ID + 1;
static constexpr size_t DRIVE_FL_ID = POWER_ID + 1;
static constexpr size_t DRIVE_FR_ID = DRIVE_FL_ID + 1;
static constexpr size_t DRIVE_RL_ID = DRIVE_FR_ID + 1;
static constexpr size_t DRIVE_RR_ID = DRIVE_RL_ID + 1;