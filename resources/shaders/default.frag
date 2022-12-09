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

//float falloff(float angle, float inner, float penumbra);
//void setAtt(Light light, out float fatt);
//void setDir(Light light, out vec3 lightdir);

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main() {
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

    // TEXTURE GEN
    vec2 st = gl_FragCoord.xy/resolution.xy;
    st.x *= resolution.x/resolution.y;
    vec3 color = vec3(0.199,0.417,0.990);

    // Scale
    st *= frequency;

    // Tile the space
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float m_dist = 10.;  // minimum distance
    vec2 m_point;        // minimum point

    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 point = random2(i_st + neighbor);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);

            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }

    // Assign a color using the closest point position
    color += 0.3 * m_dist * m_dist;

    fragColor = vec4(color, 1.f);
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
