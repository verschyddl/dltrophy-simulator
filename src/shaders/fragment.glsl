#version 330 core

precision highp float;

uniform float iTime;
uniform vec4 iRect;
uniform samplerBuffer LEDS;

#define N_LEDS 172

layout(std140) uniform TrophyDefinition {
    vec3 pos[N_LEDS];
    // TODO: could try that dynamically without N_LEDS. anyway.
    // uint leds; vec3 pos[];
};

struct RGB {
    uint r, g, b;
};

layout(std140) uniform StateBuffer {
    RGB ledColor[N_LEDS];
};

vec3 c = vec3(1, 0, -1);

vec2 iResolution = iRect.zw;
float aspectRatio = iResolution.x / iResolution.y;

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    vec3 borderColor = vec3(0.4, 0.2 + 0.2 * sin(iTime) , 0.7);
    vec3 col = mix(c.yyy, borderColor, smoothstep(0.95, 1., length(uv)));

    // int idx = int(gl_FragCoord.x + gl_FragCoord.y * textureSize(LEDS));
    int idx = int(uint(gl_FragCoord.x - iRect.x)) % N_LEDS;
    RGB led = ledColor[idx];
    col = vec3(led.r, led.g, led.b) / 255.;

    // frame
    if (max(abs(uv.x / aspectRatio), abs(uv.y)) > 0.99) {
        col = c.xxx;
    }

    gl_FragColor = vec4(col, 1.0);
}
