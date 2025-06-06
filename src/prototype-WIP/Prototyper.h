//
// Created by qm210 on 06.06.2025.
//

#ifndef DLTROPHY_SIMULATOR_PROTOTYPER_H
#define DLTROPHY_SIMULATOR_PROTOTYPER_H

#include "FX.h"
#include "bus_manager_compat.h"

/* from wled.h:
 *
 * // led fx library object
 * WLED_GLOBAL BusManager busses _INIT(BusManager());
 * WLED_GLOBAL WS2812FX strip _INIT(WS2812FX());
 */

BusCompatibilityManager busses();
WS2812FX strip();

class Prototyper {

    /*
     * Idea is: You include this and then this class behaves
     * as a WLED instance would, but connecting the above
     * globally declared WLED structures to our Trophy.
     *
     * This is being implemented... one day... maybe. we'll see.
     */

};

#endif //DLTROPHY_SIMULATOR_PROTOTYPER_H
