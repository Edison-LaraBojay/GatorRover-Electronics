#pragma once

#include <Arduino.h>

 
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