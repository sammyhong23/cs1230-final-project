#version 330 core

layout(location = 0) in vec3 ospos;
layout(location = 1) in vec3 osnorm;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec3 U;

out vec3 wspos;
out vec3 wsnorm;
out vec2 uv;

uniform mat4 ctm;
uniform mat3 invtransctm;

uniform mat4 view;
uniform mat4 proj;

uniform bool heightmapon;
uniform sampler2D heightmap;

vec3 applyHeightmap();

void main() {
    uv = uv_in;
    wspos = vec3(ctm * vec4(ospos, 1.f));

    if (heightmapon) {
        wsnorm = applyHeightmap();
    } else {
        wsnorm = invtransctm * normalize(osnorm);
    }

    gl_Position = proj * view * vec4(wspos, 1.f);
}

vec3 applyHeightmap() {
    vec2 center = uv_in;
    float uvxunit = 0.005f;
    float uvyunit = 0.005f;

    vec3 centerpos = vec3(center, texture(heightmap, center).r);

    vec2 uv1 = uv_in + vec2(       0,  uvyunit);
    vec2 uv2 = uv_in + vec2(-uvxunit,  uvyunit);
    vec2 uv3 = uv_in + vec2(-uvxunit,  0);
    vec2 uv4 = uv_in + vec2(-uvxunit, -uvyunit);
    vec2 uv5 = uv_in + vec2(       0, -uvyunit);
    vec2 uv6 = uv_in + vec2( uvxunit, -uvyunit);
    vec2 uv7 = uv_in + vec2( uvxunit,  0);
    vec2 uv8 = uv_in + vec2( uvxunit,  uvyunit);

    vec3[8] diffs;
    diffs[0] = vec3(uv1, texture(heightmap, uv1).r) - centerpos;
    diffs[1] = vec3(uv2, texture(heightmap, uv2).r) - centerpos;
    diffs[2] = vec3(uv3, texture(heightmap, uv3).r) - centerpos;
    diffs[3] = vec3(uv4, texture(heightmap, uv4).r) - centerpos;
    diffs[4] = vec3(uv5, texture(heightmap, uv5).r) - centerpos;
    diffs[5] = vec3(uv6, texture(heightmap, uv6).r) - centerpos;
    diffs[6] = vec3(uv7, texture(heightmap, uv7).r) - centerpos;
    diffs[7] = vec3(uv8, texture(heightmap, uv8).r) - centerpos;

    vec3 normsum = vec3(0);
    for (int i = 0; i < 7; ++i) {
        normsum += normalize(cross(diffs[i], diffs[i+1]));
    }
    normsum += normalize(cross(diffs[7], diffs[0]));

    vec3 W = normalize(osnorm);
    vec3 V = normalize(cross(W,U));

    mat3 UVWmat = mat3(normalize(U),V,W);

    return invtransctm * UVWmat * normalize(normsum);
}
