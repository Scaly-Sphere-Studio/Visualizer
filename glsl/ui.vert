#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProj;

out vec2 vWorldPos;

uniform vec2 uSize;
uniform vec3 uPos;

void main() {
    vec3 wPos = uPos + aPos * vec3(uSize, 1.0);
    gl_Position = uProj * vec4(wPos, 1.0);

    vWorldPos = wPos.xy;
}