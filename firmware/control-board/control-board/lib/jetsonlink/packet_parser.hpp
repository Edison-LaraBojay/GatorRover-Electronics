#pragma once

#include <Arduino.h>

#include "crc16.hpp"
#include "packet.hpp"



class PacketParser {
private:
    enum class ParsingState {
        SOF1,
        SOF2,
        RECEIVING
    };

    ParsingState state_ = ParsingState::SOF1;

    uint8_t buffer_[Packet::MAX_PACKET_SIZE] = {};

    size_t rx_index_ = 0;
    size_t expected_size_ = 0;

public:

    void reset() {
        state_ = ParsingState::SOF1;
        rx_index_ = 0;
        expected_size_ = 0;
    }

    bool process_byte(uint8_t byte, Packet& packet) {
        switch (state_) {
            case ParsingState::SOF1: {
                if (byte == Packet::SOF1) {
                    buffer_[0] = byte;
                    rx_index_ = 1;
                    state_ = ParsingState::SOF2;
                }
                return false;
            }
            case ParsingState::SOF2: {
                if (byte == Packet::SOF2) {
                    buffer_[1] = byte;
                    rx_index_ = 2;
                    state_ = ParsingState::RECEIVING;
                }
                else if (byte == Packet::SOF1) {
                    buffer_[0] = byte;
                    rx_index_ = 1;
                }
                else {
                    reset();
                }
                return false;
            }

            case ParsingState::RECEIVING: {
                if (rx_index_ >= Packet::MAX_PACKET_SIZE) {
                    reset();
                    return false;
                }

                buffer_[rx_index_++] = byte;

                // Header is complete
                if (rx_index_ == Packet::HEADER_SIZE) {
                    uint8_t length = buffer_[Packet::LENGTH_POS];

                    if (length > Packet::MAX_PAYLOAD_SIZE) {
                        reset();
                        return false;
                    }

                    expected_size_ = Packet::HEADER_SIZE + length + Packet::CRC_SIZE;
                }

                if (expected_size_ != 0 && rx_index_ == expected_size_) {
                    bool valid = deserialize(packet);
                    reset();
                    return valid;
                }

                return false;
            }
        }

        return false;
    }

private:

    bool deserialize(Packet& packet) {
        const size_t crc_index = expected_size_ - Packet::CRC_SIZE;
        uint16_t received_crc = ((uint16_t)(buffer_[crc_index + 1]) << 8) | (uint16_t)(buffer_[crc_index]);
        uint16_t calculated_crc = crc16xmodem(buffer_, crc_index);
        if (received_crc != calculated_crc) {
            return false;
        }

        packet.device_id = buffer_[Packet::DEVICE_ID_POS];
        packet.command = buffer_[Packet::COMMAND_POS];
        packet.sequence_id = buffer_[Packet::SEQUENCE_ID_POS];
        packet.length = buffer_[Packet::LENGTH_POS];
        memcpy(packet.payload, buffer_ + Packet::PAYLOAD_START_POS, packet.length);

        return true;
    }
};