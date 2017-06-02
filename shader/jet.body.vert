#version 410 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 shadowMatrix;

in vec3 vertNormal;
in vec3 vertPosition;

out vec4 Color;

out vec4 shadowCoord;
out vec3 worldCoord;
out vec3 eyeCoord;
out vec3 Normal;

void main() {
    mat3 NormalMatrix = mat3(1.0f); // current no model matrix

    vec4 position = vec4(vertPosition, 1.0f);

    vec4 worldPos = modelMatrix * position;
    vec4 eyePos = viewMatrix * worldPos;
    vec4 clipPos = projectionMatrix * eyePos;

    worldCoord = worldPos.xyz;
    eyeCoord = eyePos.xyz;
    shadowCoord = shadowMatrix * worldPos;

    gl_Position = clipPos;

    Color = vec4(0.4f, 0.4f, 0.4f, 1.0f);
    Normal = normalize(mat3(viewMatrix * modelMatrix) * vertNormal);
}