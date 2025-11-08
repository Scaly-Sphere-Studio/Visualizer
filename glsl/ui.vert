#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProj;


out vec2        iResolution;
out float       time;
out int         primSize;

uniform vec2 uSize;
uniform vec3 uPos;

void main() {
//    gl_Position = 

    vec3 wPos = uPos + aPos * vec3(uSize, 1.0);
//    vec3 wPos =aPos + uPos;
    gl_Position = uProj * vec4(wPos, 1.0);

    // Output

}