#include <Arduino.h>

#include "command.hpp"
#include "hardware.hpp"

class RoverController {
public:

    RoverController(Hardware& hw)
        : hw_(hw) {}

    void execute() {
        const Packet* request = hw_.jetsonlink.peek_packet();

        if (!request) {
            return;
        }

        switch (request->command) {

            case IMU_SAMPLE: {
                handle_imu_sample(request);
                break;
            }

            case IMU_ACCEL : {
                handle_imu_accel(request);
                break;
            }

            /// TODO: add more commands

            default: {
                break;
            }
        }

        hw_.jetsonlink.pop_packet();
    }

    void handle_imu_sample(const Packet* request) {
        Packet response(
            request->peripheral_id, 
            request->command, 
            request->sequence_id, 
            IMU_SAMPLE_PAYLOAD_SIZE
        );

        size_t offset = 0;
        const ImuSample& sample = hw_.imu.latest();
        
        memcpy(response.payload + offset, sample.accel_mps2, sizeof(sample.accel_mps2));
        offset += sizeof(sample.accel_mps2);
        memcpy(response.payload + offset, sample.gyro_radps, sizeof(sample.gyro_radps));
        offset += sizeof(sample.gyro_radps);
        memcpy(response.payload + offset, &sample.temperature_c, sizeof(sample.temperature_c));

        hw_.jetsonlink.send_packet(response);
    }

    void handle_imu_accel(const Packet* request) {
        Packet response(
            request->peripheral_id, 
            request->command, 
            request->sequence_id, 
            IMU_ACCEL_PAYLOAD_SIZE
        );

        size_t offset = 0;
        const ImuSample& sample = hw_.imu.latest();
        
        memcpy(response.payload + offset, sample.accel_mps2, sizeof(sample.accel_mps2));
        offset += sizeof(sample.accel_mps2);

        hw_.jetsonlink.send_packet(response);
    }

private:

    Hardware& hw_;
};