#version 410 core

#define TILE_RESOLUTION 128.0

uniform int worldWidth;
uniform int uEdgeMorph;
uniform float uScale;
uniform float uOffsetX;
uniform float uOffsetY;
uniform vec3 globalOffset;
uniform sampler2D uHeightData;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec2 uOffset;
float vMorphFactor;

in vec2 vertPosition;

out vec3 vPosition;
out vec2 vPosXY;

float getHeight(vec2 p) {
    float h = texture(uHeightData, p).r;
    float hh = h * h * h / 4;
    return hh;
}

void main() {
    uOffset = vec2(uOffsetX, uOffsetY);

    float grid = uScale / TILE_RESOLUTION;
    vPosition.xy = vertPosition;
    vPosition.xy = vPosition.xy * uScale + uOffset + globalOffset.xy;
    vPosition.z = 0.f;
    vec2 scaledPos = (vPosition.xy / worldWidth + 1.f) * .5f;
    vPosition.z = getHeight(scaledPos);
    vPosition = vPosition.xzy;

    gl_Position = projectionMatrix * viewMatrix * vec4(vPosition, 1.f);
    vPosition.xz = scaledPos;
}
