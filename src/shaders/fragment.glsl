#version 300 es

precision highp float;

void main() {
    // vec2 uv = fragCoord/iResolution.xy;
    // vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
    // gl_FragColor = vec4(col, 1.);
    gl_FragColor = vec4(0.4, 0.1, 0.7, 1.0);
}