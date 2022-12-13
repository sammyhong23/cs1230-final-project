#version 330 core

in vec2 uvs;

uniform sampler2D mainTexture;
uniform sampler2D flowMap;

out vec4 fragColor;

uniform float flowSpeed = 0.1;
uniform float flowIntensity = 0.1;
uniform float time;

void main()
{
    vec2 flow = texture(flowMap, uvs).xy;
    flow = (flow + 0.5f) * 5.0f;
    float time1 = fract(time * flowSpeed);
    float time2 = fract(time1 + 0.5f);
    float flowMix = abs((time1 - 0.5f) * 2.0f);

    vec4 texture1 = texture(mainTexture, uvs + (flow * time1 * flowIntensity));
    vec4 texture2 = texture(mainTexture, uvs + (flow * time2 * flowIntensity));
    fragColor = mix(texture1, texture2, flowMix);
}
