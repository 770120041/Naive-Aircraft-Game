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

void main() {
    vec3 N = Normal;
    vec3 L = normalize(LightDirection - worldCoord);
    vec3 R = reflect(-L, N);
    vec3 E = normalize(eyeCoord);

    float NdotL = dot(N, L);
    float EdotR = dot(-E, R);

    float diffuse = max(NdotL, 0.f);
    float specular = max(pow(EdotR, Shininess), 0.f);
    float f = textureProj(depthTexture, shadowCoord);

    if (f < minimalDiffuse) { f = minimalDiffuse; }

    FragColor = vec4(KaColor + f * ( KdColor * diffuse + KsColor * specular), 1.f);
}