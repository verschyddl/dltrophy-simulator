#version 330 core

precision highp float;
out vec4 fragColor;
out int clickedLedIndex;

uniform float iTime;
uniform vec4 iRect;
uniform float iFPS;
uniform vec4 iMouse;
uniform int iFrame;
uniform int iPass;
uniform sampler2D iPreviousImage;
uniform sampler2D iBloomImage;

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
    float ledSize, ledGlow;
    float camX, camY, camZ, camFov, camTilt;
    float fogScaling, fogGrading, backgroundSpin;
    float floorLevel, floorSpacingX, floorSpacingZ,
          floorLineWidth, floorExponent, floorGrading;
    float pyramidX, pyramidY, pyramidZ,
          pyramidScale, pyramidHeight,
          pyramidAngle, pyramidAngularVelocity;
    float epoxyPermittivity;
    float blendPreviousMixing;
    float traceMinDistance, traceMaxDistance;
    int traceMaxSteps, traceMaxRecursions;
    int options;
};

//const float MAX_DIST = 500.;
//const float MIN_DIST = 1.e-3;
//const int MAX_STEPS = 120;
//// Note: The Epoxy Pyramid needs Ray Tracing:
//const int MAX_RECURSION = 8;
//const float PYRAMID_STEP = 0.1; // unused yet

#define hasOption(index) (options & (1 << (8 * index))) != 0

bool showGrid = hasOption(0);
bool accumulateForever = hasOption(1);
bool noStochasticVariation = hasOption(2);
bool onlyPyramidFrame = hasOption(3);

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
const int PYRAMID_FRAME_MATERIAL = 3;
const int DEBUG_MATERIAL = 99;

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

#define clampVec3(x) clamp(x, vec3(0), vec3(1))

const vec2 epsilon = c.xz * 0.0005;

mat3 rotateX(float theta) {
    float c = cos(theta * rad);
    float s = sin(theta * rad);
    return mat3(
        vec3(1,  0,  0),
        vec3(0,  c, -s),
        vec3(0, +s,  c)
    );
}

mat3 rotateY(float theta) {
    float c = cos(theta * rad);
    float s = sin(theta * rad);
    return mat3(
        vec3( c, 0, s),
        vec3( 0, 1, 0),
        vec3(-s, 0, c)
    );
}

mat3 rotateZ(float theta) {
    float c = cos(theta * rad);
    float s = sin(theta * rad);
    return mat3(
        vec3( c, -s, 0),
        vec3(+s,  c, 0),
        vec3( 0,  0, 1)
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

//// pseudorandomosities ////

float globalSeed = 0.;

uint base_hash(uvec2 p) {
    p = 1103515245U*((p >> 1U)^(p.yx));
    uint h32 = 1103515245U*((p.x)^(p.y>>3U));
    return h32^(h32 >> 16);
}

float hash1(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    return float(n)*(1.0/float(0xffffffffU));
}

vec2 hash2(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    uvec2 rz = uvec2(n, n*48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU))/float(0x7fffffff);
}

float hash21(vec2 co){
    return fract(sin(dot(co.xy,vec2(1.9898,7.233)))*45758.5433);
}

//////

// inspiri-stolen from https://www.shadertoy.com/view/tsScRK

float starnoise(vec3 rd){
    float c = 0.;
    vec3 p = normalize(rd)*200.;
    for (float i=0.;i<4.;i++)
    {
        vec3 q = fract(p)-.5;
        vec3 id = floor(p);
        float c2 = smoothstep(.5,0.,length(q));
        c2 *= step(hash21(id.xz/id.y),.06-i*i*0.005);
        c += c2;
        p = p*.6+.5*p*mat3(3./5.,0,4./5.,0,1,0,-4./5.,0,3./5.);
    }
    c*=c;
    float g = dot(sin(rd*10.512),cos(rd.yzx*10.512));
    c*=smoothstep(-3.14,-.9,g)*.5+.5*smoothstep(-.3,1.,g);
    return c*c;
}

vec3 background(vec3 rd, vec3 ld) {
    float haze = 0.3 * exp2(-5.*(abs(rd.y)-.2*dot(rd,ld)));
    float st = 3. * starnoise(rd * rotateZ(backgroundSpin * iTime)) * (1. - min(haze,1.));
    vec3 back = vec3(0.,.1,.7)
        * exp2(-.1*abs(length(rd.xz)/rd.y))
        * max(sign(rd.y),0.);
    return clamp(
            mix(back, vec3(.3,.1, .52), haze) + st
        , 0., 1.);
}

////

vec3 floorColor = vec3(0., 0.73 - 0.2 * cos(iTime), 0.94 - 0.05 * sin(0.4 * iTime));
const vec3 floorNormal = c.yxy;

Marched sdFloor(vec3 p) {
    float d = dot(p, floorNormal) - floorLevel;
    return Marched(d, FLOOR_MATERIAL, c.xxx, -1, floorNormal);
}

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float sdLineSegment(vec3 p, vec3 a, vec3 b) {
    vec3 ap = p - a;
    vec3 ab = b - a;
    float t = clamp(dot(ap, ab) / dot(ab, ab), 0.0, 1.0);
    vec3 closest = a + t * ab;
    return length(p - closest);
}

float sdPyramidFrame(vec3 p, float h, float b) {
    // Base vertices
    vec3 v1 = vec3( b, 0,  b);
    vec3 v2 = vec3(-b, 0,  b);
    vec3 v3 = vec3(-b, 0, -b);
    vec3 v4 = vec3( b, 0, -b);
    vec3 apex = vec3(0, h, 0);

    // Base edges
    float d1 = sdLineSegment(p, v1, v2);
    float d2 = sdLineSegment(p, v2, v3);
    float d3 = sdLineSegment(p, v3, v4);
    float d4 = sdLineSegment(p, v4, v1);

    // Side edges (apex to base)
    float d5 = sdLineSegment(p, apex, v1);
    float d6 = sdLineSegment(p, apex, v2);
    float d7 = sdLineSegment(p, apex, v3);
    float d8 = sdLineSegment(p, apex, v4);

    // Minimum distance to any edge
    float frameDist = min(min(min(d1, d2), min(d3, d4)),
    min(min(d5, d6), min(d7, d8)));

    // Subtract radius for line thickness (e.g., 0.02)
    return frameDist - 0.005;
}

mat3 pyramidRotation = rotateY(pyramidAngle + pyramidAngularVelocity * iTime);
mat3 pyramidCounterRotation = rotateY(-pyramidAngle - pyramidAngularVelocity * iTime);

// thanks iq https://iquilezles.org/articles/distfunctions/
float sdPyramid( vec3 p )
{
    float h = pyramidHeight;
    p -= vec3(pyramidX, pyramidY, pyramidZ);
    p *= pyramidRotation / pyramidScale;

    if (onlyPyramidFrame) {
        return sdPyramidFrame(p, pyramidHeight, 0.5);
    }

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

Marched sdScene(vec3 p) {
    Marched hit = sdFloor(p);
    float sd;

    sd = sdPyramid(p);
    if (sd < hit.sd) {
        hit.sd = sd;
        hit.material = onlyPyramidFrame ? PYRAMID_FRAME_MATERIAL : PYRAMID_MATERIAL;
    }

    p *= pyramidRotation;
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

Marched analyticalHit(Ray ray) {
    float co = dot(ray.origin, floorNormal);
    float cd = dot(ray.dir, floorNormal);

    Marched hit = Marched(1.e4, MISS, c.xxx, -1, floorNormal);
    if (cd == 0) { // ray parallel to the floor -- can never hit
        return hit;
    }
    float distance = (floorLevel - co) / cd;
    if (distance > 0.) {
        hit.material = FLOOR_MATERIAL;
        hit.sd = distance;
    }
    return hit;
}

Marched marchScene(Ray ray) {
    Marched hit;
    float depth = traceMinDistance;
    vec3 p;

    for(int i = 0; i < traceMaxSteps; i++) {
        p = advance(ray, depth);
        hit = sdScene(p);
        depth += hit.sd;
        if (hit.sd < traceMinDistance) {
            break;
        }
        // behind or below the pyramid there is nothing interesting anymore
        if (depth >= traceMaxDistance || p.y < pyramidY - traceMinDistance) {
            return analyticalHit(ray);
        }
    }

    hit.sd = depth;
    hit.normal = calcNormal(advance(ray, hit.sd));
    return hit;
}

float schlick(float cosine, float ior) {
    float r0 = (1.-ior)/(1.+ior);
    r0 = r0*r0;
    return r0 + (1.-r0)*pow((1.-cosine),5.);
}

bool modifiedRefract(const in vec3 p, const in vec3 normal, const in float ni_over_nt, out vec3 refracted) {
    float dt = dot(p, normal);
    float discriminant = 1. - ni_over_nt*ni_over_nt*(1.-dt*dt);
    if (discriminant > 0.) {
        refracted = ni_over_nt*(p - normal*dt) - normal*sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

void pyramidScatter(const Ray ray, const Marched hit, out vec3 attenuation, out Ray scattered) {
    // this resembles dielectric part from RIOW
    vec3 normal, refracted;
    vec3 reflected = reflect(ray.dir, hit.normal);
    float ni_over_nt, cosine;

    attenuation = vec3(1.,0.,1.);
    if (dot(ray.dir, hit.normal) > 0.) {
        normal = -hit.normal;
        ni_over_nt = epoxyPermittivity;
        cosine = dot(ray.dir, hit.normal);
        cosine = sqrt(
            1. - epoxyPermittivity*epoxyPermittivity*(1.-cosine*cosine)
        );
    } else {
        normal = hit.normal;
        ni_over_nt = 1. / epoxyPermittivity;
        cosine = -dot(ray.dir, hit.normal);
    }

    float reflectProbability = 1.;
    if (modifiedRefract(ray.dir, normal, ni_over_nt, refracted)) {
        reflectProbability = schlick(cosine, epoxyPermittivity);
    }
    vec3 currentPosition = advance(ray, hit.sd);
    scattered = Ray(
        currentPosition,
        hash1(globalSeed) < reflectProbability
            ? reflected : refracted
    );
}

vec3 opaqueMaterial(Marched hit, vec3 ray) {
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
            f = max(c.yy, 1. - f + .5 * floorLineWidth);
            f = pow(f, vec2(floorExponent));
            float g = pow(f.x+f.y, 1./floorGrading);
            g = clamp(g, 0., 1.);
            const vec3 dark = 0.3 * vec3(.2, .1, .32);
            return mix(dark, floorColor, g);

        case PYRAMID_FRAME_MATERIAL:
            return vec3(0.62, 0.0, 1.);

        default:
            // Ein ulkiges Orange, kann ja aber nie(TM) vorkommen.
            return c.xwy;
    }
}

Marched traceScene(Ray ray) {
    vec3 col = c.yyy;
    Marched hit;
    Marched direct_hit;
    Ray scatRay;
    int r;

    for (r = 0; r < traceMaxRecursions; r++) {
        hit = marchScene(ray);
        if (r == 0) {
            direct_hit = hit;
        }
        if (hit.material != PYRAMID_MATERIAL) {
            hit.color = opaqueMaterial(hit, advance(ray, hit.sd));
            break;
        }
        if (hit.sd >= traceMaxDistance) {
            break;
        }
        // still here? -> hit the pyramid :) go on scattering, mr. funny boi
        vec3 attenuation;
        pyramidScatter(ray, hit, attenuation, scatRay);
        col *= attenuation;
        ray = scatRay;
    }
    if (r == traceMaxRecursions) {
        // didn't converge. bad.
        hit.material = MISS;
        hit.color = c.wyx; // some signal lilac :P
    }
    return hit;
}

void postProcess(inout vec3 col, in vec2 uv) {
    // simple vignette for now.
    float rf = length(uv) * 0.9;
    rf = pow(rf, 4.2) + 1.;
    rf = pow(rf, -1.6);
    col *= clamp(rf, 0., 1.);

    // gamma grading
    // col = pow(col, vec3(1.4));
    // gain
    // col = col * 4.0/(2.5 + col);
}

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;

    // border frame
    float uvX = uv.x / aspectRatio;
    if (max(abs(uvX), abs(uv.y)) > 0.993) {
        if (abs(uvX) >= abs(uv.y)) { // vertical frame?
            fragColor.rgb = uvX < 0 ? borderDark : borderLight;
        } else {
            fragColor.rgb = uv.y > 0 ? borderDark : borderLight;
        }
        return;
    }

    fragColor = c.xxxy;
    vec3 col = fragColor.rgb;

    vec3 grid = draw_grid(uv);
    if (showGrid) {
        fragColor.rgb = 0.5 * grid;
    }

    uvec2 bits = floatBitsToUint(gl_FragCoord.xy);
    globalSeed = float(base_hash(bits))/float(0xffffffffU);
    if (!noStochasticVariation) {
        globalSeed += iTime;
        uv += hash2(globalSeed) / iResolution;
    }

    vec3 lightDir = normalize(vec3(0,.125+.05*sin(.1*iTime),1));
    vec3 ro = vec3(camX, camY, camZ);
    vec3 rd;
    Ray ray;
    Marched hit;

    rd = normalize(vec3(uv, camFov));
    rd *= rotateX(camTilt);
    fragColor.rgb = background(rd, lightDir);

    col = c.yyy;
    ray = Ray(ro, rd);
    hit = traceScene(ray);
    float fogMixing = exp(-fogScaling * pow(abs(hit.sd), fogGrading));
    col = mix(fragColor.rgb, hit.color, fogMixing);

    bool hovered = distance(iMouse.xy, gl_FragCoord.xy) < 1.;
    bool clicked = iMouse.z > 0 && hovered;
    clickedLedIndex = clicked ? hit.ledIndex : -1;

    if (showGrid) {
        col += 0.2 * grid;
    }

    fragColor = vec4(clampVec3(col), 1.);

    // blend previous image
    vec2 st = (gl_FragCoord.xy) / (iResolution + iRect.xy);
    vec4 previousImage = texture(iPreviousImage, st);

    if (iPass == 1) {
        col = previousImage.rgb / previousImage.a;
        postProcess(col, uv);
        fragColor = vec4(col, 1.);
        return;
    }

    if (accumulateForever) {
        // forever-accumulating uses the alpha channel to count the weight.
        if (iFrame > 0) {
            fragColor += previousImage;
        }
        return;
    }

    fragColor.rgb = mix(fragColor.rgb, previousImage.rgb, blendPreviousMixing);
    fragColor.a = 1.;
}
