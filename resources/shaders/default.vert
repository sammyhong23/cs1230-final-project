#version 330 core

//layout(location = 0) in vec3 ospos;
//layout(location = 1) in vec3 osnorm;

layout(location = 0) in vec3 inPos;

//out vec3 wspos;
//out vec3 wsnorm;

out vec3 pos;

//uniform mat4 ctm;
//uniform mat3 invtransctm;

//uniform mat4 view;
//uniform mat4 proj;

void main() {
    pos = inPos;
    gl_Position = vec4(pos, 1.f);
//    wspos = vec3(ctm * vec4(ospos, 1.f));
//    wsnorm = invtransctm * normalize(osnorm);
//    gl_Position = proj * view * vec4(wspos, 1.f);
}
