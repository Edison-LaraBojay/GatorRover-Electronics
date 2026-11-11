#pragma once
#include <stdint.h>
#include <stddef.h>

// See jetsen_teensy_protocol.md for more information on packet structure.

namespace packet {
    

}



struct Packet {

    static constexpr size_t PACKET_BUFFER_SIZE = 256; // Based on sequence ID size (1 byte supports 256 packets)
    static constexpr size_t MAX_PAYLOAD_SIZE = 64;
    static constexpr size_t HEADER_SIZE = 6;
    static constexpr size_t SOF_SIZE = 2;
    static constexpr size_t CRC_SIZE = 2;
    static constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE + CRC_SIZE;


    static constexpr size_t SOF1 = 0;
    static constexpr size_t SOF2 = 1;
    static constexpr size_t PERIPHERAL_ID = 2;
    static constexpr size_t COMMAND = 3;
    static constexpr size_t SEQUENCE_ID = 4;
    static constexpr size_t LENGTH = 5;
    static constexpr size_t PAYLOAD_START = 6;


    inline static uint16_t crc16xmodem(const uint8_t* data, size_t length) {

        uint16_t crc = 0;

        for (size_t i = 0; i < length; ++i) {
            crc = crc ^ ((uint16_t)data[i] << 8);

            for (size_t bit = 0; bit < 8; ++bit) {

                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc = crc << 1;
                }
            }
        }

        return crc;
    }

    /**
     * Contains the raw data of the packet (header, payload, and CRC).
     */
    uint8_t data[MAX_PACKET_SIZE] = {};

    /**
     * Length of the payload.
     */
    size_t length = 0;

};