#version 330 core

precision highp float;
out vec4 fragColor;
out int clickedLedIndex;

uniform float iTime;
uniform vec4 iRect;
uniform float iFPS;
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

    float camX, camY, camZ, camFov, camTilt;
    float diffuseAmount, specularAmount, specularGrading;
    float fogScaling, fogGrading;
    float floorSpacingX, floorSpacingZ,
          floorLineWidth, floorExponent, floorGrading;
    float pyramidX, pyramidY, pyramidZ,
          pyramidScale, pyramidHeight,
          pyramidAngle, pyramidAngularVelocity;

    int options;
};

#define hasOption(index) (options & (1 << index)) != 0

bool showGrid = hasOption(0);
bool debug1 = hasOption(1);
bool debug2 = hasOption(2);
bool debug3 = hasOption(3);

vec3 to_vec(RGB rgb) {
    return vec3(rgb.r, rgb.g, rgb.b) / 255.;
}

const float pi = 3.14159265;
const float rad = pi / 180.;
const float tau = 2. * pi;
const vec4 c = vec4(1., 0., -1., .5);

const int MISS = -1;
const int LED_MATERIAL = 0;
const int FLOOR_MATERIAL = 1;
const int PYRAMID_MATERIAL = 2;

const vec3 borderDark = vec3(0.4);
const vec3 borderLight = vec3(0.6);

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

#define clampVec3(x) clamp(x, vec3(0), vec3(1));

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

mat3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3( c, 0, s),
        vec3( 0, 1, 0),
        vec3(-s, 0, c)
    );
}

struct Marched {
    float sd;
    int material;
    vec3 color;
    int ledIndex;
    vec3 normal;
};

struct Ray {
    vec3 origin;
    vec3 dir;
};

vec3 advance(Ray ray, float depth) {
    return ray.origin + depth * ray.dir;
}

vec3 floorColor = vec3(0., 0.73 - 0.2 * cos(iTime), 0.94 - 0.05 * sin(0.4 * iTime));

Marched sdFloor(vec3 p, float level) {
    float d = p.y - level;
    return Marched(d, FLOOR_MATERIAL, c.yyy, -1, c.xyx);
}

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

// thanks iq https://iquilezles.org/articles/distfunctions/
float sdPyramid( vec3 p )
{
    float h = pyramidHeight;
    p -= vec3(pyramidX, pyramidY, pyramidZ);
    p *= rotateY(pyramidAngle + pyramidAngularVelocity * iTime) / pyramidScale;

    float m2 = h*h + 0.25;

    p.xz = abs(p.xz);
    p.xz = (p.z>p.x) ? p.zx : p.xz;
    p.xz -= 0.5;

    vec3 q = vec3( p.z, h*p.y - 0.5*p.x, h*p.x + 0.5*p.y);

    float s = max(-q.x,0.0);
    float t = clamp( (q.y-0.5*p.z)/(m2+0.25), 0.0, 1.0 );

    float a = m2*(q.x+s)*(q.x+s) + q.y*q.y;
    float b = m2*(q.x+0.5*t)*(q.x+0.5*t) + (q.y-m2*t)*(q.y-m2*t);

    float d2 = min(q.y,-q.x*m2-q.y*0.5) > 0.0 ? 0.0 : min(a,b);

    return sqrt( (d2+q.z*q.z)/m2 ) * sign(max(q.z,-p.y));
}

// RAY MARCHING / TRACING

const float MAX_DIST = 1.e3;
const float MIN_DIST = 1.e-3;
const int MAX_STEPS = 200;
// Note: The Epoxy Pyramid needs some Ray Tracing:
const float PYRAMID_STEP = 0.1;
const int MAX_RECURSION = 6;

#define MARCH_SDF(SDF, MAT) sd = SDF; if (sd < hit.sd) { hit.sd = sd; hit.material = MAT; }

Marched sdScene(vec3 p) {
    Marched hit = sdFloor(p, -4.);
    float sd;

    MARCH_SDF(sdPyramid(p), PYRAMID_MATERIAL)

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

Marched marchScene(Ray ray) {
    Marched hit;
    float depth = MIN_DIST;

    for(int i = 0; i < MAX_STEPS; i++) {
        hit = sdScene(advance(ray, depth));
        depth += hit.sd;
        if (hit.sd < MIN_DIST || depth > MAX_DIST) {
            break;
        }
    }

    hit.sd = depth;
    return hit;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

bool pyramidScatter(const Ray ray, const Marched hit, out vec3 attenuation, out Ray scattered) {
    // we only need the dielectric part from RIOW 1.09
    vec3 outward_normal, refracted;
    vec3 reflected = reflect(ray.dir, rec.normal);
    float ni_over_nt, reflect_prob, cosine;

    attenuation = vec3(1);
    if (dot(ray.dir, rec.normal) > 0.) {
        outward_normal = -rec.normal;
        ni_over_nt = rec.mat.v;
        cosine = dot(ray.dir, rec.normal);
        cosine = sqrt(1. - rec.mat.v*rec.mat.v*(1.-cosine*cosine));
    } else {
        outward_normal = rec.normal;
        ni_over_nt = 1. / rec.mat.v;
        cosine = -dot(ray.dir, rec.normal);
    }

    if (modified_refract(ray.dir, outward_normal, ni_over_nt, refracted)) {
        reflect_prob = schlick(cosine, rec.mat.v);
    } else {
        reflect_prob = 1.;
    }

    if (hash1(g_seed) < reflect_prob) {
        scattered = ray(rec.p, reflected);
    } else {
        scattered = ray(rec.p, refracted);
    }
    return true;
}

vec3 nonPyramidColor(Marched hit, vec3 ray) {
    switch (hit.material) {
        case LED_MATERIAL:
            return to_vec(ledColor[hit.ledIndex]);
        case FLOOR_MATERIAL:
            // Synthwave Grid ftw.
            vec2 f = vec2(
            fract(ray.x / floorSpacingX),
            fract(ray.z / floorSpacingZ)
            );
            f = .5 - abs(.5 - f);
            f = max(vec2(0), 1. - f + .5*floorLineWidth);
            f = pow(f, vec2(floorExponent));
            float g = pow(f.x+f.y, 1./floorGrading);
            g = clamp(g, 0., 1.);
            return mix(hit.color, floorColor, g);
        default:
            // Signalfarbe (Orange), kann ja aber nie(TM) vorkommen.
            return c.xwy;
    }
}

Marched traceScene(Ray ray) {
    vec3 col = c.yyy;
    Marched hit;
    Ray scatRay;

    for (int r=0; r < MAX_RECURSION; r++) {
        hit = marchScene(ray);
        if (d >= MAX_DIST) {
            hit.material = MISS;
        }
        if (hit.material != PYRAMID_MATERIAL) {
            hit.color = nonPyramidColor(hit, advance(ray, d));
            return hit;
        }
        // still here? -> hit the pyramid :) is gonne be fun.
        if (pyramidScatter(ray, hit, attenuation, scatRay)) {
            col *= attenuation;
            ray = scatRay;
        } else {
            hit.color = c.yyw; // is that right to discard?? choose alarming color (pissgelb)
            return hit;
        }
    }
    hit.material = MISS;
    hit.color = c.xxx;
    return hit;
}

Marched oldMarcher(vec3 ro, vec3 rd) {
    float d = hit.sd;
    if (d >= MAX_DIST) {
        hit.material = MISS;
        return hit;
    }

    vec3 col, normal, refl, sunPos, sunDir;
    ray = ro + rd * d;
    col = nonPyramidColor(hit, ray);
    normal = calcNormal(ray);

    // sunPos = ... egal, gleichmäßig paralleles Licht:
    sunDir = normalize(vec3(0.3, 2., 1.));

    float diffuse = dot(normal, sunDir);
    diffuse = clamp(diffuse, 0., 1.);
    d = mix(d, diffuse, diffuseAmount);

    refl = reflect(sunDir, normal);
    float specular = dot(refl, normalize(ray));
    specular = clamp(specular, 0., 1.);
    specular = pow(specular, specularGrading);
    d = mix(d, specular, specularAmount);

    col = mix(fragColor.rgb, col, exp(-fogScaling * pow(hit.sd, fogGrading)));
    hit.color = col;
    return hit;
}

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    // TODO:
    // float seed = float(base_hash(floatBitsToUint(frag_coord)))/float(0xffffffffU)+iTime;
    // uv += hash2(seed) / iResolution;
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

    vec3 ro = vec3(camX, camY, camZ);
    vec3 rd = normalize(vec3(uv, camFov));
    rd *= rotateX(camTilt * rad);
    Ray ray = Ray(ro, rd);
    Marched hit = traceScene(ray);
    col = hit.color;

    bool hovered = distance(iMouse.xy, gl_FragCoord.xy) < 1.;
    bool clicked = iMouse.z > 0 && hovered;
    clickedLedIndex = clicked ? hit.ledIndex : -1;

    if (showGrid) {
        col += 0.2 * grid;
    }

    fragColor.rgb = clampVec3(col);
}
