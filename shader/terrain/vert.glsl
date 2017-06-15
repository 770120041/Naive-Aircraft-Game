#version 410 core

#define TILE_RESOLUTION 128.0

uniform int uEdgeMorph;
uniform float uScale;
uniform vec2 uOffset;
uniform vec3 globalOffset;
uniform sampler2D uHeightData;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec2 vertPosition;
out vec3 color;

out vec3 vPosition;
out float vMorphFactor;

#define EGDE_MORPH_TOP 1
#define EGDE_MORPH_LEFT 2
#define EGDE_MORPH_BOTTOM 4
#define EGDE_MORPH_RIGHT 8

// Poor man's bitwise &
bool edgePresent(int edge) {
  int e = int(uEdgeMorph) / edge;
  return 2 * ( e / 2 ) != e;
}

#define MORPH_REGION 0.3

// At the edges of tiles morph the vertices, if they are joining onto a higher layer
float calculateMorph(vec2 p) {
  float morphFactor = 0.0;
  if( edgePresent(EGDE_MORPH_TOP) && p.y >= 1.0 - MORPH_REGION ) {
    float m = 1.0 - clamp((1.0 - p.y) / MORPH_REGION, 0.0, 1.0);
    morphFactor = max(m, morphFactor);
  }
  if( edgePresent(EGDE_MORPH_LEFT) && p.x <= MORPH_REGION ) {
    float m = 1.0 - clamp(p.x / MORPH_REGION, 0.0, 1.0);
    morphFactor = max(m, morphFactor);
  }
  if( edgePresent(EGDE_MORPH_BOTTOM) && p.y <= MORPH_REGION ) {
    float m = 1.0 - clamp(p.y / MORPH_REGION, 0.0, 1.0);
    morphFactor = max(m, morphFactor);
  }
  if( edgePresent(EGDE_MORPH_RIGHT) && p.x >= 1.0 - MORPH_REGION ) {
    float m = 1.0 - clamp((1.0 - p.x) / MORPH_REGION, 0.0, 1.0);
    morphFactor = max(m, morphFactor);
  }

  return morphFactor;
}

float getHeight(vec2 p) {
    float h = texture(uHeightData, p).r;
    return h * h / 64 + 256 + 512;
}

void main() {
    vMorphFactor = calculateMorph(vertPosition);

    float grid = uScale / TILE_RESOLUTION;

    if (vMorphFactor > 0.f) {
        grid = 2.f * grid;
        vec2 pos2 = floor(vertPosition / grid) * grid;

        vPosition.xy = mix(vertPosition, pos2, vMorphFactor);
    } else {
        vPosition.xy = vertPosition;
    }
    float tt = getHeight(vPosition.xy);
    vPosition = vec3(vPosition.xy * uScale + uOffset + globalOffset.xy, tt).xzy;
    color = (abs(vec3((vPosition)) / 1024));
    gl_Position = projectionMatrix * viewMatrix * vec4(vPosition, 1.f);
}
