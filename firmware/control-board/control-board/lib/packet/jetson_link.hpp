#pragma once

#include <Arduino.h>
#include "connectors.hpp"
#include "device.hpp"
#include "packet.hpp"

class JetsonLink : public Device {
private:


public:

    explicit JetsonLink(const char* name, const JetsonConnector& uart)
        : Device(name), uart_(uart) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }


    void update(uint64_t now_us) override {
        (void)now_us;

        int byte = uart_.serial.read();

        if (byte < 0) {
            return;
        }

        process_byte((uint8_t)byte);


        /* MIGHT BE BLOCKING
        while (uart_.serial.available() > 0) {
            int byte = uart_.serial.read();

            if (byte < 0) {
                break;
            }

            process_byte(static_cast<uint8_t>(byte));
        }
        */

    }

    void process_byte(uint8_t byte) {

        // Queue is full
        if (queue_count_ >= PACKET_BUFFER_SIZE) {
            return;
        }

        Packet& curr_packet = packet_buffer_[queue_head_];

        // Search for SOF1
        if (rx_index_ == Packet::SOF1) {
            if (byte != 0xAA) {
                return;
            }
        }

        // Search for SOF2
        else if (rx_index_ == Packet::SOF2) {
            if (byte != 0x55) {
                rx_index_ = (byte == 0xAA) ? 1 : 0;
                return;
            }
        }

        if (rx_index_ >= Packet::MAX_PACKET_SIZE) {
            rx_index_ = 0;
            return;
        }

        curr_packet.data[rx_index_] = byte;

        if (rx_index_ == Packet::LENGTH) {
            curr_packet.length = byte;

            if (curr_packet.length > Packet::MAX_PAYLOAD_SIZE) {
                rx_index_ = 0;
                return;
            }
        }

        ++rx_index_;

        if (rx_index_ < Packet::HEADER_SIZE) {
            return;
        }

        size_t expected_size = Packet::HEADER_SIZE + curr_packet.length + Packet::CRC_SIZE;

        if (rx_index_ < expected_size) {
            return;
        }

        size_t crc_index = expected_size - Packet::CRC_SIZE;

        uint16_t crc_received =
            (uint16_t)(curr_packet.data[crc_index]) |
            ((uint16_t)(curr_packet.data[crc_index + 1]) << 8);

        // Calculate CRC over header + payload
        uint16_t crc_calculated = Packet::crc16xmodem(curr_packet.data, crc_index);

        // Reset receive position for next packet
        rx_index_ = 0;

        if (crc_received != crc_calculated) {
            return;
        }

        // Enqueue valid packet
        queue_head_ = (queue_head_ + 1) % PACKET_BUFFER_SIZE;
        ++queue_count_;
    }


    void execute() {

    }

private:

    static constexpr size_t PACKET_BUFFER_SIZE = 8;

    const JetsonConnector& uart_;

    Packet packet_buffer_[PACKET_BUFFER_SIZE];
    size_t packet_index_ = 0;
    uint8_t rx_buffer_[Packet::MAX_PACKET_SIZE] = {};
    size_t rx_index_ = 0;

    size_t queue_head_ = 0;
    size_t queue_tail_ = 0;
    size_t queue_count_ = 0;
};