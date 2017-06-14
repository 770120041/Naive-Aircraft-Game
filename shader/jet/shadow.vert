#version 410 core

uniform mat4 MVPMatrix;

in vec3 vertPosition;

void main() {
    gl_Position = MVPMatrix * vec4(vertPosition, 1.f);
}