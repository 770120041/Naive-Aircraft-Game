#version 410 core

uniform mat4 MVPMatrix;
// uniform mat3 NormalMatrix;

in vec3 vertNormal;
in vec3 vertPosition;

out vec4 Color;
out vec3 Normal;

void main() {
    mat3 NormalMatrix = mat3(1.0f); // current no model matrix
    gl_Position = MVPMatrix * vec4(vertPosition, 1.0f);
    Color = vec4(0.4f, 0.4f, 0.4f, 1.0f);
    Normal = normalize(NormalMatrix * vertNormal);
}