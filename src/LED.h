//
// Created by qm210 on 26.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_LED_H
#define DLTROPHY_SIMULATOR_LED_H

#ifdef WORKAROUND_GL
#define GLuint unsigned int
#else
#include <GL/gl.h>
#endif


struct LED {

public:
    // this struct must match a multiple of 4 bytes
    // to be passed directly into the uniform buffers.
    GLuint r;
    GLuint g;
    GLuint b;
    [[maybe_unused]] GLuint _unusedAlignment = 0;

    LED(GLuint R, GLuint G, GLuint B)
    : r(R), g(G), b(B) {}

    explicit LED(GLuint W)
    : LED(W, W, W) {}

    LED(): LED(0) {}

    void set(LED led) {
        r = led.r;
        g = led.g;
        b = led.b;
    }

    void set(GLuint w) {
        r = w;
        g = w;
        b = w;
    }

    [[nodiscard]]
    GLuint gray() const {
        auto w = 0.299 * r + 0.587 * g + 0.114 * b;
        return static_cast<GLuint>(w);
    }
};

#endif //DLTROPHY_SIMULATOR_LED_H
