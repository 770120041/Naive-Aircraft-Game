#version 410 core

uniform sampler2DShadow depthTexture;

uniform vec3 Ambient;
uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 HalfVector;

uniform float Shininess;
uniform float Strength;

in vec4 Color;

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

    FragColor = vec4(vec3(Color) + f * (.2f * diffuse + .2f * specular), 1.f);
}