//
// Created by qm210 on 06.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_BUS_COMPAT_H
#define DLTROPHY_SIMULATOR_BUS_COMPAT_H

#include "bus_compat.h"

#include "../../Trophy.h"
#include "../../ShaderState.h"

class BusCompatibilityManager {

    BusCompatibilityManager() = default;
    ~BusCompatibilityManager() = default;

    void initialize(Trophy* trophy, ShaderState* state) {
        trophy_ = trophy;
        state_ = state;
    };

    Bus* getBus(uint8_t busNr) {
        if (busNr > busses.size()) {
            return nullptr;
        }
        return busses[busNr];
    }

private:
    Trophy* trophy_;
    ShaderState* state_;

    std::vector<Bus*> busses{};
};

#endif //DLTROPHY_SIMULATOR_BUS_COMPAT_H
