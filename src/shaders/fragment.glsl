#version 330 core

precision highp float;

uniform float iTime;
uniform vec4 iRect;
uniform samplerBuffer LEDS;

layout(std140, binding=0) uniform ledBuffer {
    vec3 ledArray[180];
} iLEDs;

vec3 c = vec3(1,0,-1);

vec2 iResolution = iRect.zw;

void main() {
    vec2 uv = (2. * (gl_FragCoord.xy - iRect.xy) - iResolution) / iResolution.y;
    vec3 borderColor = vec3(0.4, 0.2 + 0.2 * sin(iTime) , 0.7);
    vec3 col = mix(c.yyy, borderColor, smoothstep(0.95, 1., length(uv)));

    // int idx = int(gl_FragCoord.x + gl_FragCoord.y * textureSize(LEDS));
    uint idx = uint(gl_FragCoord.x);
    col = texelFetch(LEDS, int(idx)).rgb;

    gl_FragColor = vec4(col, 1.0);
}
