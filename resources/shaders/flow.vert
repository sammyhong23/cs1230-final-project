#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvCoords;

out vec2 uvs;

void main() {
    uvs = uvCoords;
    gl_Position = vec4(position, 1.0);
}
