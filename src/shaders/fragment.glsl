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
bool debug2 = (options & (1 << 2)) != 0;
bool debug3 = (options & (1 << 3)) != 0;

vec3 to_vec(RGB rgb) {
    return vec3(rgb.r, rgb.g, rgb.b) / 255.;
}

vec4 c = vec4(1., 0., -1., .5);

vec2 iResolution = iRect.zw;
float aspectRatio = iResolution.x / iResolution.y;

void draw_grid(out vec3 col, in vec2 uv) {
    if (!showGrid) {
        return;
    }
    float thickness = 3.e-3;
    const float step = 0.1;
    uv = abs(uv);
    col = c.yyy;
    if (min(uv.x, uv.y) < 1.5 * thickness) {
        thickness *= 1.5;
        col.g = 0.8;
        col.b = 1.;
    }
    else if (mod(uv.x, step) < thickness ||
             mod(uv.y, step) < thickness) {
        col.g = 0.6;
    }
}

// 3D GEOMETRY

struct Marched {
    float sd;
    int material;
    int ledIndex;
};

const int MISS = -1;
const int LED_MATERIAL = 0;

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

const float MAX_DIST = 1.e3;
const float MIN_DIST = 1.e-3;
// Fixed step size ray marching - required inside the Epoxy Pyramid
const float STEP_SIZE = 0.1;
const int MAX_STEPS = 100;

void marchScene(out Marched hit, vec3 p) {
    hit = Marched(MAX_DIST, MISS, -1);
    float sd;

    for (int i = 0; i < nLeds; i++) {
        sd = sdSphere(p, ledPosition[i].xyz, ledSize);
        if (sd < hit.sd) {
            hit.sd = sd;
            hit.ledIndex = i;
        }
    }
}

Marched marchRay(vec3 ro, vec3 rd) {
    Marched hit = Marched(MAX_DIST, MISS, -1); // <-- Initialisierung eher überflüssig...
    float depth = MIN_DIST;

    for(int i = 0; i < MAX_STEPS; i++) {
        marchScene(hit, ro + rd * depth);
        depth += hit.sd;

//        if (hit.sd < STEP_SIZE) {
//            col += vec3(1.0);
//        } else {
//            float glow = exp(-hit.sd * 2.0);
//            col += glow * 0.2;
//        }
//        depth += STEP_SIZE;
        if (hit.sd < MIN_DIST || depth > MAX_DIST) {
            break;
        }
    }

    hit.sd = depth;
    if (hit.ledIndex >= 0) {
        hit.material = LED_MATERIAL;
    }
    return hit;
}

vec3 borderDark = vec3(0.4);
vec3 borderLight = vec3(0.6);

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    fragColor = c.xxxy;

    if (max(abs(uv.x / aspectRatio), abs(uv.y)) > 0.99) { // frame ?
        if (abs(uv.x) >= abs(uv.y)) { // vertical frame?
            fragColor.rgb = uv.x < 0 ? borderDark : borderLight;
        } else {
            fragColor.rgb = uv.y > 0 ? borderDark : borderLight;
        }
        return;
    }

    vec3 col, grid_color;
    draw_grid(grid_color, uv);
    col = 0.3 * grid_color;

    if (debug3) {
        // Test 2D Rendering only
        float d;
        for (int i = 0; i < nLeds; i++) {
            d = distance(uv, ledPosition[i].xy);
            d = exp(-pow(abs(d), ledExponent) / ledSize);
            col += d * to_vec(ledColor[i]);
        }
        fragColor = vec4(col, 1.0);
        return;
    }

    vec3 ro = vec3(0, 0, -1);
    vec3 rd = normalize(vec3(uv, 1.0));
    Marched hit = marchRay(ro, rd);

    if (hit.sd > MAX_DIST) {

    }

    fragColor = vec4(col, 1.0);
}
