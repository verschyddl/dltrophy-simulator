//
// Created by qm210 on 25.07.2025.
//

#ifndef DLTROPHY_SIMULATOR_PROTOTYPER_H
#define DLTROPHY_SIMULATOR_PROTOTYPER_H

/**
 * This class aims to connect FX_DEADLINE_TROPHY.h with the Simulator
 * by "shimming" all required definitions and global variables in WLED.

 * therefore the explicit includes:
 *  - FX_DEADLINE_TROPHY.h = obviously the thing we want to do its magic
 *  - wled.h = our hollow shell of WLED, a shadow of its former strength
 *  - DeadlineTrophy.h = in the best case, an exact copy / symlink to the header in the firmware
 */

#include "wled.h"
#include "../usermods/DEADLINE_TROPHY/DeadlineTrophy.h"

#include <chrono>

typedef uint16_t (*mode_ptr)();


class Prototyper {
private:
    bool enabled = false;
    std::chrono::steady_clock::time_point startedAt;

    static mode_ptr _mode;

public:
    explicit Prototyper(bool enabled)
    : enabled(enabled)
    {
        // TODO: can I somehow check whether the sources are even available,
        //       and if not, overwrite enable to always false? -- find out.

        setUpDeadlineTrophy();

        startedAt = std::chrono::steady_clock::now();
    }

    /**
     *  this mocks(*) FX_fcn.cpp / WS2812FX::service()
     *  but I didn't want the WS2812FX polyfill to contain much logic,
     *  rather expose everything it has and let the Prototyper() be in control.
     *
     *  (*) as in, making fun of. No, but really,
     *      I figured it best to keep the structure as original as I could,
     *      but declared some stuff not-as-interesting-right-now
     *      TODO: check whether we should care about the SKIPPED parts.
     */
    void service() const {
        if (!enabled) {
            return;
        }

        // SKIPPED: most of the timing calculaterino at the begningning

        auto nowUp = millis();
        strip.now = nowUp + strip.timebase;

        bool doShow = false;

        strip._segment_index = 0;
        for (auto &seg : strip._segments)
        {
            // SKIPPED: check whether isActive()

            if (nowUp > seg.next_time) {
                doShow = true;
                unsigned frameDelay = FRAMETIME;

                if (!seg.freeze) {
                    beginDraw(seg);
                    strip._currentSegment = &seg;

                    // This calls the actual FX, equals:
                    // frameDelay = (*_mode[seg.mode])();
                    frameDelay = _mode();

                    seg.call++;

                    // SKIPPED: segment blending / transitions
                }

                seg.next_time = nowUp + frameDelay;
            }
            strip._segment_index++;
        }

        if (doShow) {
            // SKIPPED: Segment::handleRandomPalette()
            strip._lastServiceShow = nowUp; // unclear whether used
            show();
        }
    }

    void toggle() {
        enabled ^= true;

        if (enabled) {
            restart();
        }
    }

    void restart() {
        strip.timebase = 0UL - millis();
        for (auto &seg : strip._segments) {
            seg.call = 0;
        }
    }

    unsigned long millis() const {
        // Note: being an unsigned long, this overflows every 49 days, but so does it on the device
        auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - startedAt
                );
        return static_cast<unsigned long>(elapsed.count());
    }

private:

    void show() const {
        // resembles WS2812FX::show(), due to some unexplainable coincidence

        // TDOO - BIG, FAT TODO!!
    }

    void setUpDeadlineTrophy() const {
        strip.isMatrix = true;
        strip._segments.assign(
                DeadlineTrophy::segment,
                DeadlineTrophy::segment + DeadlineTrophy::N_SEGMENTS
        );
        strip._length = DeadlineTrophy::N_LEDS_TOTAL;
        Segment::maxWidth = DeadlineTrophy::logoW;
        Segment::maxHeight = DeadlineTrophy::logoH + DeadlineTrophy::baseSize;
            // <-- could just be logoH, right? (the larger one of these)

        for (size_t i=0; i < DeadlineTrophy::N_SEGMENTS; i++) {
            Segment seg = strip._segments[i];
            // seg.setName(DeadlineTrophy::segmentName[i]); // no need to port that, yet
            seg.colors[0] = 0xAA00FF;
            seg.colors[1] = 0xFF40FF;
            seg.colors[2] = 0xFFFFFF;
        }
    }

    void beginDraw(const Segment& seg) const {
        // SKIPPED: the transition progress argument (uint16_t prog = 0xFFFFU)
        setDrawDimensions(seg);
        for (unsigned i = 0; i < NUM_COLORS; i++) {
            Segment::_currentColors[i] = seg.colors[i];
        }
        // SKIPPED: loadPalette
        // SKIPPED: transition stuff
    }

    inline void setDrawDimensions(const Segment& seg) const {
        Segment::_vWidth = seg.virtualWidth();
        Segment::_vHeight = seg.virtualHeight();
        Segment::_vLength = seg.virtualLength();
    }

};

#endif //DLTROPHY_SIMULATOR_PROTOTYPER_H
