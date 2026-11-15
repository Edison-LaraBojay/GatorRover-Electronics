#pragma once

#include <Arduino.h>
#include "connectors.hpp"
#include "device.hpp"
#include "packet.hpp"
#include "packet_parser.hpp"
#include "packet_serializer.hpp"

class JetsonLink : public Device {
private:
    const JetsonConnector& uart_;

    PacketParser parser_;

    static constexpr size_t PACKET_BUFFER_SIZE = 5;
    Packet packet_buffer_[PACKET_BUFFER_SIZE];

    size_t queue_head_ = 0;
    size_t queue_tail_ = 0;
    size_t queue_count_ = 0;

public:
    explicit JetsonLink(const char* name, const JetsonConnector& uart)
        : Device(name), uart_(uart) {}

    bool begin() override {
        status_ = DeviceStatus::Ok;
        return true;
    }

    void update(uint64_t now_us) override {
        (void)now_us;

        while (uart_.serial.available() > 0) {
            int byte = uart_.serial.read();

            if (byte < 0) {
                break;
            }

            Packet packet;

            if (parser_.process_byte((uint8_t)(byte), packet)) {
                if (queue_count_ < PACKET_BUFFER_SIZE) {
                    packet_buffer_[queue_head_] = packet;

                    queue_head_ =
                        (queue_head_ + 1) % PACKET_BUFFER_SIZE;

                    queue_count_++;
                }
            }
        }
    }

    void send_packet(const Packet& packet) {
        uint8_t buffer[Packet::MAX_PACKET_SIZE];

        size_t length = PacketSerializer::serialize(
            packet,
            buffer,
            sizeof(buffer)
        );

        if (length == 0) {
            return;
        }

        uart_.serial.write(buffer, length);
    }
};