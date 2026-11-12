#include "packet.hpp"

#include <stdint.h>
#include <stddef.h>


// --- Transport Frequencies --- //

static constexpr uint32_t kI2cClockHz = 400000;
static constexpr uint32_t kJetsonBaud = 115200;
static constexpr uint32_t kLidarBaud  = 115200;


// --- Device/Peripheral IDs --- //

static constexpr size_t JETSON   = 0;
static constexpr size_t IMU      = 1;
static constexpr size_t LIDAR    = 2;
static constexpr size_t POWER    = 3;
static constexpr size_t DRIVE_FL = 4;
static constexpr size_t DRIVE_FR = 5;
static constexpr size_t DRIVE_RL = 6;
static constexpr size_t DRIVE_RR = 7;


// --- Command Table --- //

// IMU
static constexpr u_int8_t IMU_SAMPLE = 0;
static constexpr size_t   IMU_SAMPLE_PAYLOAD_SIZE = 4;
static constexpr size_t   IMU_SAMPLE_PACKET_SIZE = Packet::HEADER_SIZE + IMU_SAMPLE_PAYLOAD_SIZE + Packet::CRC_SIZE;