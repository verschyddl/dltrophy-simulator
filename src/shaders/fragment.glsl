#version 330 core

precision highp float;
out vec4 fragColor;
out int clickedLedIndex;

uniform float iTime;
uniform vec4 iRect;
uniform vec4 iMouse;

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

const float pi = 3.14159265;
const float rad = pi / 180.;
const float tau = 2. * pi;
const vec4 c = vec4(1., 0., -1., .5);

vec2 iResolution = iRect.zw;
float aspectRatio = iResolution.x / iResolution.y;

vec3 draw_grid(in vec2 uv) {
    vec3 col = c.yyy;
    if (!showGrid) {
        return col;
    }
    float thickness = 3.e-3;
    const float step = 0.1;
    uv = abs(uv);
    if (min(uv.x, uv.y) < 1.5 * thickness) {
        thickness *= 1.5;
        col.g = 0.8;
        col.b = 1.;
    }
    else if (mod(uv.x, step) < thickness ||
             mod(uv.y, step) < thickness) {
        col.g = 0.6;
    }
    return col;
}

// 3D GEOMETRY

const vec2 epsilon = c.xz * 0.0005;

mat3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1,  0,  0),
        vec3(0,  c, -s),
        vec3(0, +s,  c)
    );
}

struct Marched {
    float sd;
    int material;
    int ledIndex;
};

const int MISS = -1;
const int LED_MATERIAL = 0;
const int FLOOR_MATERIAL = 1;

Marched sdFloor(vec3 p, float level) {
    float d = p.y - level;
    return Marched(d, FLOOR_MATERIAL, -1);
}

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

// RAY MARCHING

const float MAX_DIST = 1.e3;
const float MIN_DIST = 1.e-3;
const int MAX_STEPS = 400;
// Note: Fixed step size required inside the Epoxy Pyramid
const float STEP_SIZE = 0.1;

Marched sdScene(vec3 p) {
    Marched hit = sdFloor(p, -4.);

    float sd;
    for (int i = 0; i < nLeds; i++) {
        sd = sdSphere(p, ledPosition[i].xyz, ledSize);
        if (sd < hit.sd) {
            hit.sd = sd;
            hit.ledIndex = i;
        }
    }
    if (hit.ledIndex >= 0) {
        hit.material = LED_MATERIAL;
    }
    return hit;
}

vec3 calcNormal(in vec3 p) {
    return normalize(
        epsilon.xyy * sdScene(p + epsilon.xyy).sd +
        epsilon.yyx * sdScene(p + epsilon.yyx).sd +
        epsilon.yxy * sdScene(p + epsilon.yxy).sd +
        epsilon.xxx * sdScene(p + epsilon.xxx).sd
    );
}

Marched marchRay(vec3 ro, vec3 rd) {
    Marched hit = Marched(MAX_DIST, MISS, -1); // Initialisierung eher überflüssig. ja.
    float depth = MIN_DIST;

    for(int i = 0; i < MAX_STEPS; i++) {
        hit = sdScene(ro + rd * depth);
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
    return hit;
}

vec3 floorColor = vec3(0., 0.73 - 0.2 * cos(iTime), 0.94 - 0.05 * sin(0.4 * iTime));

vec3 materialColor(Marched hit, vec3 ray) {
    switch (hit.material) {
        case LED_MATERIAL:
            return to_vec(ledColor[hit.ledIndex]);
        case FLOOR_MATERIAL:
            vec2 spacing = vec2(5.21);
            vec2 grid = mod(ray.xz, spacing);
            vec2 dist = min(grid, spacing - grid);
            float width = .3;
            return floorColor * clamp(width - min(dist.x, dist.y), 0., 1.);
        default:
            return c.xyy; // Signalfarbe Rot, weil darf nicht sein.
    }
}

#define clampVec3(x) clamp(x, vec3(0), vec3(1));

vec3 borderDark = vec3(0.4);
vec3 borderLight = vec3(0.6);

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    fragColor = c.yyyx;
    vec3 col = fragColor.rgb;

    if (max(abs(uv.x / aspectRatio), abs(uv.y)) > 0.99) { // frame ?
        if (abs(uv.x) >= abs(uv.y)) { // vertical frame?
            fragColor.rgb = uv.x < 0 ? borderDark : borderLight;
        } else {
            fragColor.rgb = uv.y > 0 ? borderDark : borderLight;
        }
        return;
    }

    vec3 grid = draw_grid(uv);
    if (showGrid) {
        fragColor.rgb = 0.5 * grid;
    }

    if (debug3) {
        // Test 2D Rendering only
        float d;
        for (int i = 0; i < nLeds; i++) {
            d = distance(uv, ledPosition[i].xy);
            d = exp(-pow(abs(d), ledExponent) / ledSize);
            col += d * to_vec(ledColor[i]);
        }
        fragColor.rgb = clampVec3(col);
        return;
    }

    const vec3 cameraPosition = vec3(0., 0.17, -1.8);
    const float fieldOfView = 1.3;
    const float cameraTilt = 12.3;

    vec3 ro = cameraPosition;
    vec3 rd = normalize(vec3(uv, fieldOfView));
    rd *= rotateX(cameraTilt * rad);
    vec3 ray, normal, refl, sunPos, sunDir;

    const float diffuseAmount = 0.7;
    const float specularAmount = 0.4;
    const float specularGrading = 2.1;

    Marched hit = marchRay(ro, rd);
    float d = hit.sd;
    if (d < MAX_DIST) {
        ray = ro + rd * d;
        col = materialColor(hit, ray);
        normal = calcNormal(ray);

        // sunPos = ... egal, gleichmäßig paralleles Licht:
        vec3 sunDir = normalize(vec3(0.3, 2., 1.));

        float diffuse = dot(normal, sunDir);
        diffuse = clamp(diffuse, 0., 1.);
        d = mix(d, diffuse, diffuseAmount);

        refl = reflect(sunDir, normal);
        float specular = dot(refl, normalize(ray));
        specular = clamp(specular, 0., 1.);
        specular = pow(specular, specularGrading);
        d = mix(d, specular, specularAmount);

        // col = mix(fragColor.rgb, col, exp(-0.0001 * pow(hit.sd, 3.)));
    }

    bool hovered = distance(iMouse.xy, gl_FragCoord.xy) < 1.;
    bool clicked = iMouse.z > 0 && hovered;
    clickedLedIndex = clicked ? hit.ledIndex : -1;

    if (showGrid) {
        col += 0.2 * grid;
    }

    fragColor.rgb = clampVec3(col);
}
