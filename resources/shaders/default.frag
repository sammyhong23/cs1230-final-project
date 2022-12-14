#version 330 core

struct Light {
  int type; // 0 - directional, 1 - point, 2 - spot, -1 - invalid
  vec3 pos;
  vec3 dir;
  vec4 color;
  vec3 function;
  float angle;
  float penumbra;
};

in float height;

in vec3 wspos;
in vec3 wsnorm;
in vec2 uv;

out vec4 fragColor;

uniform float ka;
uniform float kd;
uniform float ks;

uniform float n;
uniform vec4 oa;
uniform vec4 od;
uniform vec4 os;

uniform vec3 camerapos;
uniform Light lights[8];

float falloff(float angle, float inner, float penumbra);
void setAtt(Light light, out float fatt);
void setDir(Light light, out vec3 lightdir);

uniform bool texturemapon;
uniform sampler2D texturemap;
uniform sampler2D flowMap;
uniform float flowSpeed;

uniform sampler2D heightmap;

uniform float time;

uniform bool worley;

vec4 flowmap(sampler2D texmap, vec2 uvcoord);

void main() {
    fragColor = ka * oa;
    float ndotl;
    for (int i = 0; i < 8; ++i) {
        float fatt;
        vec3 lightdir;

        setAtt(lights[i], fatt);
        setDir(lights[i], lightdir);

        vec4 diffuse;
        vec4 specular;
        float power;

        ndotl = dot(-lightdir, normalize(wsnorm));

        if (ndotl > 0) {
            diffuse = (texturemapon) ? flowmap(texturemap, uv) : kd * od;

            vec3 ri = reflect(lightdir, normalize(wsnorm));
            float rdotv = dot(ri, normalize(camerapos - wspos));
            power = (n > 0) ? pow(rdotv, n) : 1;
            specular = (rdotv > 0) ? ks * os : vec4(0.f);
            fragColor += fatt * lights[i].color * (diffuse * ndotl + specular * power);
        }
    }

    fragColor = clamp(fragColor, 0.f, 1.f);
}

float falloff(float angle, float inner, float penumbra) {
    return -2 * pow((angle - inner) / penumbra, 3) + 3 * pow((angle - inner) / penumbra, 2);
}

void setAtt(Light light, out float fatt) {
    float dist;
    float curangle;
    float inner;

    switch(light.type) {
    case 0:
        fatt = 1;
        break;
    case 1:
        dist = length(wspos - light.pos);
        fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
        break;
    case 2:
        dist = length(wspos - light.pos);
        curangle = acos(dot(normalize(wspos - light.pos), normalize(light.dir)));
        inner = light.angle - light.penumbra;

        if (curangle <= inner) {
            fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
        } else if (curangle > inner && curangle <= inner + light.penumbra) {
            fatt = min(1.f, 1 / (light.function[0] + dist * light.function[1] + pow(dist, 2) * light.function[2]));
            fatt = fatt * (1.f - falloff(curangle, inner, light.penumbra));
        } else {
            fatt = 0.f;
        }
        break;
    default:
        fatt = 0;
        break;
    }
}

void setDir(Light light, out vec3 lightdir) {
    lightdir = (light.type == 0) ? normalize(light.dir) : normalize(wspos - light.pos);
}

vec4 flowmap(sampler2D texmap, vec2 uvcoord) {
    if (time == 0) {
        return texture(texmap, uvcoord);
    }
    float time1 = fract(time * flowSpeed);
    float time2 = fract(time1 + 0.5f);
    float flowMix = abs((time1 - 0.5f) * 2.0f);

    //vec2 flow = texture(flowMap, uvcoord).rg;
    vec2 flow = vec2(0.f, 0.5);
    flow *= 5.f;

    vec4 texture1 = texture(texmap, uvcoord + (flow * time1 * 0.1));
    vec4 texture2 = texture(texmap, uvcoord + (flow * time2 * 0.1));

    float scale = 1.f;

    return mix(texture1, texture2, flowMix) * scale;
}


