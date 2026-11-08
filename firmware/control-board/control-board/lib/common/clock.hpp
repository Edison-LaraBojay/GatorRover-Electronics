#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace sysclock {

// 64-bit microsecond clock built on micros(), which wraps every ~71.6 minutes.
// Must be called at least once per wrap period (the main loop does this).
// Not ISR-safe: call from the main loop only.
inline uint64_t nowUs() {
    static uint32_t last = 0;
    static uint64_t high = 0;
    const uint32_t t = micros();
    if (t < last) {
        high += (1ULL << 32);
    }
    last = t;
    return high | t;
}

}  // namespace sysclock
