#version 330 core

precision highp float;
out vec4 fragColor;

uniform float iTime;
uniform vec4 iRect;

const int nLeds = 172;

layout(std140) uniform TrophyDefinition {
    int _nLeds; // TODO: doesn't work - care about later
    vec4 ledPosition[nLeds];
};

struct RGB {
    // we just have RGB LEDs, so do not confuse the 4th component
    // which still needs to be aligned for the GPU, so -> 4 fields
    uint r, g, b, _;
};

layout(std140) uniform StateBuffer {
    RGB ledColor[nLeds];
};

vec3 to_vec(RGB rgb) {
    return vec3(rgb.r, rgb.g, rgb.b) / 255.;
}

vec3 c = vec3(1, 0, -1);

vec2 iResolution = iRect.zw;
float aspectRatio = iResolution.x / iResolution.y;

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;

    // border frame
    if (max(abs(uv.x / aspectRatio), abs(uv.y)) > 0.99) {
        fragColor = c.xxxy;
        return;
    }

    vec3 col, pos;
    float d;
    for (int i = 0; i < nLeds; i++) {
        col = to_vec(ledColor[i]);
        pos = ledPosition[i].xyz;

        // test rendering
        d = distance(uv, pos.xy);
        col *= exp(-0.1 * pow(d, 2));
    }

    RGB led = RGB(25u, 100u, 210u, 0u);
    float n = ceil(sqrt(float(nLeds)));
    if (uv.x < 0 || uv.y < 0) {
        discard;
    }
    int idx = int(n * uv.x);
    int idy = int(n * uv.y);
    int i = int(float(idy) * n) + idx;
    led = ledColor[i];
    col = to_vec(led);

    fragColor = vec4(col, 1.0);
}
