//
// Created by qm210 on 30.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_MESSAGES_H
#define DLTROPHY_SIMULATOR_MESSAGES_H

#include <algorithm>
#include <ranges>
#include <functional>
#include <thread>
#include <vector>
#include "Range.h"

struct RGB {
    uint8_t r = 0, g = 0, b = 0;

    template <typename typeR, typename typeG, typename typeB>
    RGB(typeR r, typeG g, typeB b)
    : r(static_cast<uint8_t>(r)),
      g(static_cast<uint8_t>(g)),
      b(static_cast<uint8_t>(b))
      {}

      RGB() = default;
};

const uint8_t FIRST_LOGO_INDEX = 0;
const uint8_t N_LOGO = 106;
const uint8_t FIRST_BASE_INDEX = N_LOGO;
const uint8_t N_BASE = 64;

const uint8_t WARLS_HEADER = 1;
const uint8_t DRGB_HEADER = 2;

const int timeout_sec = 255;

inline std::vector<uint8_t> createWARLS(
        const std::vector<uint8_t>& indices,
        const std::function<RGB(size_t)>& func
) {
    std::vector<uint8_t> message{
        WARLS_HEADER,
        timeout_sec
    };
    for (const auto& index: indices) {
        auto led = func(index);
        message.insert(message.end(), {
                static_cast<uint8_t>(index),
                led.r,
                led.g,
                led.b
        });
    }
    return message;
}

inline std::vector<uint8_t> createDRGB(
        const std::vector<uint8_t>& indices,
        const std::function<RGB(size_t)>& func
) {
    std::vector<uint8_t> message{
        DRGB_HEADER,
        timeout_sec
    };
    for (const auto& index: indices) {
        auto led = func(index);
        message.insert(message.end(), {led.r, led.g, led.b});
    }
    return message;
}

inline auto asBytes(std::vector<int> const& range) {
    return std::vector<uint8_t>(range.begin(), range.end());
}

inline auto rangeOfAll() {
    auto range = Range::range(0, N_LOGO + N_BASE + 2);
    return asBytes(range);
}

inline auto rangeOfLogo() {
    auto range = Range::range(FIRST_LOGO_INDEX, N_LOGO);
    return asBytes(range);
}

inline std::vector<uint8_t> rangeOfBase(int edge = -1) {
    const int nEdge = N_BASE / 4;
    std::vector<int> range;
    switch (edge) {
        case 0:
            range = Range::range(FIRST_BASE_INDEX, nEdge);
            break;
        case 1:
        case 2:
            range = Range::range(FIRST_BASE_INDEX + nEdge + edge - 1, nEdge, 2);
            break;
        case 3:
            range = Range::range(FIRST_BASE_INDEX + 3 * nEdge, nEdge);
            break;
        default:
            range = Range::range(FIRST_BASE_INDEX, N_BASE);
            break;
    }
    return asBytes(range);
}

inline std::vector<uint8_t> rangeOfSingleLeds() {
    return asBytes(
            Range::range(N_LOGO + N_BASE, 2)
    );
}

#endif //DLTROPHY_SIMULATOR_MESSAGES_H
