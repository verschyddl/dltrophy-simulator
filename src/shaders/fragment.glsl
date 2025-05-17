#version 330 core

precision highp float;

uniform float iTime;
uniform vec4 iRect;
uniform samplerBuffer LEDS;

#define N_LEDS 172

struct RGB {
    uint r, g, b;
};

layout(std140) uniform RGBBuffer {
    // wir geben nur 3 uint8_t rein, aber... alignment
    // -> lies uvec4, was auch immer dann in der 4. Komponente steht.
    // uvec4 ledArray[180];
    // oder... hm... let's see...
    RGB ledArray[N_LEDS];
};

vec3 c = vec3(1, 0, -1);

vec2 iResolution = iRect.zw;

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    vec3 borderColor = vec3(0.4, 0.2 + 0.2 * sin(iTime) , 0.7);
    vec3 col = mix(c.yyy, borderColor, smoothstep(0.95, 1., length(uv)));

    // int idx = int(gl_FragCoord.x + gl_FragCoord.y * textureSize(LEDS));
    int idx = int(uint(gl_FragCoord.x)) % N_LEDS;
    RGB led = ledArray[idx];
    col = vec3(led.r, led.g, led.b) / 255.;

    gl_FragColor = vec4(col, 1.0);
}
