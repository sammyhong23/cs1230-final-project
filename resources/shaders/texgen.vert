#version 330 core

layout(location = 0) in vec3 ospos;

out vec3 pos;

void main() {
    pos = ospos;
    gl_Position = vec4(pos, 1.f);
}
