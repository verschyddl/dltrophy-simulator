//
// Created by qm210 on 15.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_TIMEFORMAT_H
#define DLTROPHY_SIMULATOR_TIMEFORMAT_H

#include <string>
#include <chrono>

static inline std::string formatTime(std::chrono::system_clock::time_point now = std::chrono::system_clock::now()) {
    struct tm tm{};
    auto time = std::chrono::system_clock::to_time_t(now);

    // to unite cross-compatibility and thread-safety -- this abomination.
    bool failed = true;
#if defined(_WIN32) || defined(_WIN64)
    failed &= localtime_s(&tm, &time) != 0;
#elif defined(__linux__) || defined(__unix__)
    failed &= localtime_r(&time, &tm) == nullptr;
#endif

    if (failed) {
        return "TIMESTAMP_ERROR";
    }
    char result[32];
    strftime(result, sizeof(result), "%Y-%m-%d %H:%M:%S", &tm);
    auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()
            ).count() % 1000;
    return std::format("{}.{:03d}", result, ms);
}

#endif //DLTROPHY_SIMULATOR_TIMEFORMAT_H
