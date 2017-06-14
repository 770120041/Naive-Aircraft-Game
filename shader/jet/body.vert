#version 410 core

#define MaxMtls 50

uniform vec3 Ka[MaxMtls];
uniform vec3 Kd[MaxMtls];
uniform vec3 Ks[MaxMtls];

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 shadowMatrix;

in float materialId;
in vec3 vertNormal;
in vec3 vertPosition;

out vec3 KaColor;
out vec3 KdColor;
out vec3 KsColor;

out vec4 shadowCoord;
out vec3 worldCoord;
out vec3 eyeCoord;
out vec3 Normal;

void main() {
    int id = int(materialId);
    mat3 NormalMatrix = mat3(1.0f); // current no model matrix

    vec4 position = vec4(vertPosition, 1.0f);

    vec4 worldPos = modelMatrix * position;
    vec4 eyePos = viewMatrix * worldPos;
    vec4 clipPos = projectionMatrix * eyePos;

    worldCoord = worldPos.xyz;
    eyeCoord = eyePos.xyz;
    shadowCoord = shadowMatrix * worldPos;

    gl_Position = clipPos;

    //Color = vec4(vec3(Ka[id]), 1.f);
    KaColor = Ka[id];
    KdColor = Kd[id];
    KsColor = Ks[id];
    Normal = normalize(mat3(viewMatrix * modelMatrix) * vertNormal);
}