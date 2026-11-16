#pragma once

#include <Arduino.h>


struct Packet {

    // Packet byte positions
    static constexpr size_t SOF1_POS = 0;
    static constexpr size_t SOF2_POS = SOF1_POS + 1;
    static constexpr size_t PERIPHERAL_ID_POS = SOF2_POS + 1;
    static constexpr size_t COMMAND_POS = PERIPHERAL_ID_POS + 1;
    static constexpr size_t SEQUENCE_ID_POS = COMMAND_POS + 1;
    static constexpr size_t LENGTH_POS = SEQUENCE_ID_POS + 1;
    static constexpr size_t PAYLOAD_START_POS = LENGTH_POS + 1;

    // Packet metadata
    static constexpr uint8_t SOF1 = 0xAA;
    static constexpr uint8_t SOF2 = 0x55;
    static constexpr size_t MAX_PAYLOAD_SIZE = 64;
    static constexpr size_t HEADER_SIZE = PAYLOAD_START_POS;
    static constexpr size_t SOF_SIZE = 2;
    static constexpr size_t CRC_SIZE = 2;
    static constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE + CRC_SIZE;


    Packet(uint8_t p, uint8_t c, uint8_t s, uint8_t l)
        : peripheral_id(p), command(c), sequence_id(s), length(l) {}


    uint8_t peripheral_id;
    uint8_t command;
    uint8_t sequence_id;
    uint8_t length;

    uint8_t payload[MAX_PAYLOAD_SIZE];
};