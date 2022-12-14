#version 330 core

in vec3 pos;

out vec4 fragColor;

// TEXTURE GEN
uniform vec2 resolution;
uniform float frequency;
uniform float stretch;
uniform float time;
uniform bool worley;

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

void main() {
    // TEXTURE GEN
    if (worley) {
        // worley
        vec2 st = gl_FragCoord.xy/resolution.xy;
        st.x *= resolution.x/resolution.y;
        vec3 color = vec3(0);

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

        // Assign a color using the closest point position
        color += 0.3 * m_dist * m_dist;

        fragColor = vec4(color, 1.f);
    } else {
        // perlin
        vec2 st = gl_FragCoord.xy/resolution.xy;
        st.x *= resolution.x/resolution.y;
        st.x /= stretch;
        vec3 color = vec3(0.0);

        vec2 pos = vec2(st * frequency);

        // octaves
        float z1 = noise(pos);
        float z2 = noise(pos * 2) / 2;
        float z4 = noise(pos * 4) / 4;
        float z8 = noise(pos * 8) / 8;

        color = vec3((z1 + z2 + z4 + z8) * 0.5 + 0.5);

        fragColor = vec4(color, 1.f);
    }
}
