#version 410 core

#define minimalDiffuse 0.3f

uniform sampler2DShadow depthTexture;

uniform vec3 Ambient;
uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 HalfVector;

uniform float Shininess;
uniform float Strength;

in vec3 KaColor;
in vec3 KdColor;
in vec3 KsColor;

in vec4 shadowCoord;
in vec3 worldCoord;
in vec3 eyeCoord;

in vec3 Normal;

out vec4 FragColor;



float textureLookup(vec4 coord) {
    float res = 0;
    vec3 texelSize = vec3(1500.f);
    for (int x=-1; x<=1; x++) {
        for (int y=-1; y<=1; y++) {
            for (int z=-1; z<=1; z++) {
                vec4 off = vec4(vec3(x, y, z) / texelSize, 0.f);
                res += textureProj(depthTexture, coord + off);
            }
        }
    }
    res /= 27.f;
    return res;
}


void main() {
    vec3 N = Normal;
    vec3 L = normalize(LightDirection - worldCoord);
    vec3 R = reflect(-L, N);
    vec3 E = normalize(eyeCoord);

    float NdotL = dot(N, L);
    float EdotR = dot(-E, R);

    float diffuse = max(NdotL, 0.f);
    float specular = max(pow(EdotR, Shininess), 0.f);
    //float f = textureProj(depthTexture, shadowCoord);
    float f = textureLookup(shadowCoord);

    if (f < minimalDiffuse) { f = minimalDiffuse; }

    FragColor = vec4(KaColor + f * ( KdColor * diffuse + KsColor * specular) * 0.5, 1.f);
}