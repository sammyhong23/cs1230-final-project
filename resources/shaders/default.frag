#version 330 core

//struct Light {
//  int type; // 0 - directional, 1 - point, 2 - spot, -1 - invalid
//  vec3 pos;
//  vec3 dir;
//  vec4 color;
//  vec3 function;
//  float angle;
//  float penumbra;
//};

//in vec3 wspos;
//in vec3 wsnorm;
in vec3 pos;

out vec4 fragColor;

//uniform float ka;
//uniform float kd;
//uniform float ks;

//uniform float n;
//uniform vec4 oa;
//uniform vec4 od;
//uniform vec4 os;

//uniform vec3 camerapos;
//uniform Light lights[8];


// TEXTURE GEN
uniform vec2 resolution;
uniform float frequency;
uniform float stretch;
uniform float time;
uniform bool parallax;

//float falloff(float angle, float inner, float penumbra);
//void setAtt(Light light, out float fatt);
//void setDir(Light light, out vec3 lightdir);

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
    if (!parallax) {
        // worley
        vec2 st = gl_FragCoord.xy/resolution.xy;
        st.x *= resolution.x/resolution.y;
        vec3 color = vec3(0.199,0.417,0.990);

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

        vec2 uv = gl_FragCoord.xy / resolution.xy;
        uv.x *= resolution.x/resolution.y;
        uv *= 5;
        float scale = noise(uv)* 0.3 + 0.8;

        color *= scale;

        fragColor = vec4(color, 1.f);
    }

//    fragColor = ka * oa;
//    float ndotl;
//    for (int i = 0; i < 8; ++i) {
//        float fatt;
//        vec3 lightdir;

//        setAtt(lights[i], fatt);
//        setDir(lights[i], lightdir);

//        vec4 diffuse;
//        vec4 specular;
//        float power;

//        ndotl = dot(-lightdir, normalize(wsnorm));

//        if (ndotl > 0) {
//            diffuse = kd * od;
//            vec3 ri = reflect(lightdir, normalize(wsnorm));
//            float rdotv = dot(ri, normalize(camerapos - wspos));
//            power = (n > 0) ? pow(rdotv, n) : 1;
//            specular = (rdotv > 0) ? ks * os : vec4(0.f);
//            fragColor += fatt * lights[i].color * (diffuse * ndotl + specular * power);
//        }
//    }

//    fragColor = clamp(fragColor, 0.f, 1.f);
}

//float falloff(float angle, float inner, float penumbra) {
//    return -2 * pow((angle - inner) / penumbra, 3) + 3 * pow((angle - inner) / penumbra, 2);
//}

//void setAtt(Light light, out float fatt) {
//    float dist;
//    float curangle;
//    float inner;

//    switch(light.type) {
//    case 0:
//        fatt = 1;
//        break;
//    case 1:
//        dist = length(wspos - light.pos);
//        fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
//        break;
//    case 2:
//        dist = length(wspos - light.pos);
//        curangle = acos(dot(normalize(wspos - light.pos), normalize(light.dir)));
//        inner = light.angle - light.penumbra;

//        if (curangle <= inner) {
//            fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
//        } else if (curangle > inner && curangle <= inner + light.penumbra) {
//            fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
//            fatt = fatt * (1.f - falloff(curangle, inner, light.penumbra));
//        } else {
//            fatt = 0.f;
//        }
//        break;
//    default:
//        fatt = 0;
//        break;
//    }
//}

//void setDir(Light light, out vec3 lightdir) {
//    lightdir = (light.type == 0) ? normalize(light.dir) : normalize(wspos - light.pos);
//}
