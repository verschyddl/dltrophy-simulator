#version 330 core

precision highp float;

uniform float iTime;
uniform vec4 iRect;

vec3 c = vec3(1,0,-1);

void main() {
    vec2 uv = (2. * gl_FragCoord.xy - iResolution) / iResolution.y;
    vec3 borderColor = vec3(0.4, 0.2 + 0.2 * sin(iTime) , 0.7);
    vec3 col = mix(c.yyy, borderColor, smoothstep(0.95, 1., length(uv)));
    gl_FragColor = vec4(col, 1.0);
}
