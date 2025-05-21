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

struct RGB { // last value is only for alignment, never used.
    uint r, g, b, _;
};

layout(std140) uniform StateBuffer {
    RGB ledColor[nLeds];
    int options;
};

bool showGrid = (options & (1 << 0)) != 0;

vec3 to_vec(RGB rgb) {
    return vec3(rgb.r, rgb.g, rgb.b) / 255.;
}

vec3 c = vec3(1, 0, -1);

vec2 iResolution = iRect.zw;
float aspectRatio = iResolution.x / iResolution.y;

void draw_grid(out vec3 col, in vec2 uv) {
    if (!showGrid) {
        return;
    }

    float thickness = 3.e-3;
    const float step = 0.1;
    uv = abs(uv);
    // Nulllinien aussparen.
    if (min(uv.x, uv.y) < 0.25 * step) {
        return;
    }

    if (mod(uv.x, step) < thickness) {
        col.r = col.b = 1.;
    }
    if (mod(uv.y, step) < thickness) {
        col.g = 1.;
    }
}

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;

    // border frame
    if (max(abs(uv.x / aspectRatio), abs(uv.y)) > 0.99) {
        fragColor = c.xxxy;
        return;
    }

    vec3 col, pos;

    draw_grid(col, uv);

    float d;
    int nOnly = 106;
    for (int i = 0; i < nOnly; i++) {
        pos = ledPosition[i].xyz;

        // test rendering
        d = distance(uv, pos.xy);
        d = exp(-90. * pow(abs(d), 1.4));
        col += d * to_vec(ledColor[i]);
    }
    fragColor = vec4(col, 1.0);
    return;

    RGB led = RGB(25u, 100u, 210u, 0u);
    float n = ceil(sqrt(float(nLeds)));
    float nx = floor(nLeds / n);
    int idx = int(floor(nx * uv.x));
    int idy = int(floor(n * uv.y));
    int i = int(float(idy) * nx) + idx;
    if (idx >= 0 && idy >= 0) {
        led = ledColor[i];
    }
    col = to_vec(led);

    fragColor = vec4(col, 1.0);
}
