#pragma once

#include <Arduino.h>

#include "packet.hpp"
#include "crc16.hpp"

struct PacketSerializer {
    static size_t serialize(const Packet& packet, uint8_t* buffer, size_t buffer_size) {
        if (packet.length > Packet::MAX_PAYLOAD_SIZE) {
            return 0;
        }

        const size_t packet_size = Packet::HEADER_SIZE + packet.length + Packet::CRC_SIZE;

        if (buffer_size < packet_size) {
            return 0;
        }

        buffer[Packet::SOF1_POS] = Packet::SOF1;
        buffer[Packet::SOF2_POS] = Packet::SOF2;
        buffer[Packet::PERIPHERAL_ID_POS] = packet.peripheral_id;
        buffer[Packet::COMMAND_POS] = packet.command;
        buffer[Packet::SEQUENCE_ID_POS] = packet.sequence_id;
        buffer[Packet::LENGTH_POS] = packet.length;
        
        memcpy(buffer + Packet::PAYLOAD_START_POS, packet.payload, packet.length);

        const size_t crc_index = Packet::PAYLOAD_START_POS + packet.length;
        const uint16_t crc = crc16xmodem(buffer, crc_index);
        buffer[crc_index] = (uint8_t)(crc & 0xFF);
        buffer[crc_index + 1] = (uint8_t)(crc >> 8);

        return packet_size;
    }
};