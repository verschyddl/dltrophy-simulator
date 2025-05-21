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
    float ledSize;
    float ledExponent;
    int options;
};

bool showGrid = (options & (1 << 0)) != 0;
bool debug1 = (options & (1 << 1)) != 0;
// ...

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
    // Nulllinien dicker.
    if (min(uv.x, uv.y) < 2. * thickness) {
        thickness *= 2.;
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
    for (int i = 0; i < nLeds; i++) {
        pos = ledPosition[i].xyz;

        // test rendering
        d = distance(uv, pos.xy);
        d = exp(-pow(abs(d), ledExponent) / ledSize);
        col += d * to_vec(ledColor[i]);
    }

    fragColor = vec4(col, 1.0);
}

/*
#version 330 core

out vec4 FragColor;

uniform vec2 resolution;
uniform float time;

struct Light {
    vec3 pos;
    vec3 color;
};

Light lights[5];

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float map(vec3 p) {
    float result = 1000.0;

    for(int i = 0; i < 5; i++) {
        float d = sdSphere(p, lights[i].pos, 0.3);
        result = min(result, d);
    }

    return result;
}

vec3 marchRay(vec3 ro, vec3 rd) {
    float depth = 0.0;
    vec3 col = vec3(0.0);

    // Fixed step size ray marching
    const float STEP_SIZE = 0.1;
    const int MAX_STEPS = 64;

    for(int i = 0; i < MAX_STEPS && depth < 20.0; i++) {
        vec3 pos = ro + rd * depth;

        // Calculate distance to nearest light sphere
        float dist = map(pos);

        // Accumulate color based on proximity to lights
        if(dist < STEP_SIZE) {
            col += vec3(1.0); // White glow
        } else {
            // Add ambient glow effect
            float glow = exp(-dist * 2.0);
            col += glow * 0.2;
        }

        depth += STEP_SIZE;
    }

    return col;
}

void main() {
    // Initialize lights with different colors
    lights[0] = Light(vec3(-2.0, 0.0, -2.0), vec3(1.0, 0.0, 0.0)); // Red
    lights[1] = Light(vec3(2.0, 0.0, -2.0), vec3(0.0, 1.0, 0.0)); // Green
    lights[2] = Light(vec3(0.0, 2.0, 0.0), vec3(0.0, 0.0, 1.0)); // Blue
    lights[3] = Light(vec3(-2.0, 0.0, 2.0), vec3(1.0, 1.0, 0.0)); // Yellow
    lights[4] = Light(vec3(2.0, 0.0, 2.0), vec3(0.0, 1.0, 1.0)); // Cyan

    // Calculate ray direction based on pixel position
    vec2 uv = (gl_FragCoord.xy - 0.5 * resolution) / resolution.y;
    vec3 ro = vec3(0.0); // Camera position
    vec3 rd = normalize(vec3(uv, 1.0));

    // March the ray and get the final color
    vec3 col = marchRay(ro, rd);

    FragColor = vec4(col, 1.0);
}
*/