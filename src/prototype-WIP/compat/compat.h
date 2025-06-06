//
// Created by qm210 on 06.06.2025.
//
// This is the "compability header" for the actual effects header.
// i.e. for your FX_deadlineTrophy.h, to supply what otherwise
// WLED would give you, to be interchangeable without change.
//

#ifndef DLTROPHY_SIMULATOR_COMPAT_H
#define DLTROPHY_SIMULATOR_COMPAT_H

#include <cstdint>
typedef uint8_t byte;

#define PSTR(s) (s);
#define PROGMEM

#define PI M_PI 3.14159265
#define TWO_PI (2.*PI)

#include <chrono>
static inline uint32_t millis() {
    static const auto start = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return static_cast<uint32_t>(duration.count());
}

#ifndef FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_END
#define FASTLED_USING_NAMESPACE
#endif

#endif //DLTROPHY_SIMULATOR_COMPAT_H
