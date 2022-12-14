#version 330 core

layout(location = 0) in vec3 ospos;
layout(location = 1) in vec3 osnorm;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec3 U;

out float height;

out vec3 wspos;
out vec3 wsnorm;
out vec2 uv;

uniform mat4 ctm;
uniform mat3 invtransctm;

uniform mat4 view;
uniform mat4 proj;

uniform bool heightmapon;
uniform sampler2D heightmap;

uniform sampler2D flowMap;
uniform float flowSpeed;
vec4 flowmap(sampler2D texmap, vec2 uvcoord);

uniform vec2 resolution;
uniform float frequency;
uniform float stretch;
uniform bool worley;
uniform float time;

vec3 applyHeightmap();
float texturegen(vec2 coord);
float noise(vec2 st);

void main() {
    uv = uv_in;
    wspos = vec3(ctm * vec4(ospos, 1.f));

    if (heightmapon) {
        wsnorm = applyHeightmap();
    } else {
        wsnorm = invtransctm * normalize(osnorm);
    }

    height = texturegen(uv);

    gl_Position = proj * view * vec4(wspos, 1.f);
}

////// bump mapping
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
    if (true) {
        diffs[0] = vec3(uv1, flowmap(heightmap, uv1).r) - centerpos;
        diffs[1] = vec3(uv2, flowmap(heightmap, uv2).r) - centerpos;
        diffs[2] = vec3(uv3, flowmap(heightmap, uv3).r) - centerpos;
        diffs[3] = vec3(uv4, flowmap(heightmap, uv4).r) - centerpos;
        diffs[4] = vec3(uv5, flowmap(heightmap, uv5).r) - centerpos;
        diffs[5] = vec3(uv6, flowmap(heightmap, uv6).r) - centerpos;
        diffs[6] = vec3(uv7, flowmap(heightmap, uv7).r) - centerpos;
        diffs[7] = vec3(uv8, flowmap(heightmap, uv8).r) - centerpos;
    } else {
        diffs[0] = vec3(uv1, texturegen(uv1)) - centerpos;
        diffs[1] = vec3(uv2, texturegen(uv2)) - centerpos;
        diffs[2] = vec3(uv3, texturegen(uv3)) - centerpos;
        diffs[3] = vec3(uv4, texturegen(uv4)) - centerpos;
        diffs[4] = vec3(uv5, texturegen(uv5)) - centerpos;
        diffs[5] = vec3(uv6, texturegen(uv6)) - centerpos;
        diffs[6] = vec3(uv7, texturegen(uv7)) - centerpos;
        diffs[7] = vec3(uv8, texturegen(uv8)) - centerpos;
    }

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


///////////////////////////////////////deprecated texture gen stuff/////////////////////////////////////////////////////////////
vec2 worleyRandom2(vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1, 311.7)),dot(p,vec2(269.5, 183.3)))) * 43758.5453);
}

vec2 perlinRandom2(vec2 p) {
    p = vec2( dot(p,vec2(127.1, 311.7)), dot(p,vec2(269.5, 183.3)));
        return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( perlinRandom2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( perlinRandom2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( perlinRandom2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( perlinRandom2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

float texturegen(vec2 coord) {
    if (worley) {
            // worley
            vec2 st = coord;
            st.x *= resolution.x/resolution.y;

            // Scale
            st *= frequency;

            // Tile the space
            vec2 i_st = floor(st);
            vec2 f_st = fract(st);

            float m_dist = 10.;  // minimum distance

            for (int j=-1; j<=1; j++ ) {
                for (int i=-1; i<=1; i++ ) {
                    vec2 neighbor = vec2(float(i),float(j));
                    vec2 point = worleyRandom2(i_st + neighbor);
                    point = 0.5 + 0.5*sin(0.4*0.001*time + 5*point);
                    vec2 diff = neighbor + point - f_st;
                    float dist = length(diff);

                    m_dist = min(m_dist, dist);
                }
            }

            return 0.3 * m_dist * m_dist;
        } else {
            // perlin
            vec2 st = coord;
            st.x *= resolution.x/resolution.y;
            st.x /= stretch;

            vec2 pos = vec2(st * frequency);

            // octaves
            float z1 = noise(pos);
            float z2 = noise(pos * 2) / 2;
            float z4 = noise(pos * 4) / 4;
            float z8 = noise(pos * 8) / 8;

            return (z1 + z2 + z4 + z8) * 0.5 + 0.5;
        }
}

/////////////////////////// flow map stuff

vec4 flowmap(sampler2D texmap, vec2 uvcoord) {
    if (time == 0) {
        return texture(texmap, uvcoord);
    }
    float time1 = fract(time * flowSpeed);
    float time2 = fract(time1 + 0.5f);
    float flowMix = abs((time1 - 0.5f) * 2.0f);

    //vec2 flow = texture(flowMap, uvcoord).rg;
    vec2 flow = vec2(0.f, 1.f);
    flow *= 5.f;

    vec4 texture1 = texture(texmap, uvcoord + (flow * time1 * 0.1));
    vec4 texture2 = texture(texmap, uvcoord + (flow * time2 * 0.1));

    vec2 scalePos = uvcoord * 18.f;
    scalePos.x /= 2.f;

    vec2 scaleFlow = vec2(10.f, 10.f);


    float scale1 = noise(scalePos + (scaleFlow * time1 * 0.2)) * 1.3 + 1.0;
    float scale2 = noise(scalePos + (scaleFlow * time2 * 0.2)) * 1.3 + 1.0;

    return mix(texture1, texture2, flowMix) * mix(scale1, scale2, flowMix);
}
