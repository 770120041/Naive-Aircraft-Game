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
out vec3 vertColor;

#define EGDE_MORPH_TOP 1
#define EGDE_MORPH_LEFT 2
#define EGDE_MORPH_BOTTOM 4
#define EGDE_MORPH_RIGHT 8

// Poor man's bitwise &
bool edgePresent(int edge) {
  int e = uEdgeMorph / edge;
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
    float hh = h * h * h / 4;
    return hh;
}

void main() {
    uOffset = vec2(uOffsetX, uOffsetY);

    float grid = uScale / TILE_RESOLUTION;
    vMorphFactor = calculateMorph(vertPosition);
    vPosition.xy = vertPosition;
    vPosition.xy = vPosition.xy * uScale + uOffset + globalOffset.xz;
    vPosition.z = 0.f;

    vec3 morphPos = vPosition;

      if( vMorphFactor > 0.0 ) {
        // Get position that we would have if we were on higher level grid
        grid = 2.0 * grid;
        vec3 position2 = floor(morphPos / grid) * grid;

        // Linearly interpolate the two, depending on morph factor
        morphPos = mix(morphPos, position2, vMorphFactor);
      }

    vec2 scaledPos = (vPosition.xy / worldWidth + 1.f) * .5f;
    vPosition.z = getHeight(scaledPos);
    vPosition = vPosition.xzy;

    gl_Position = projectionMatrix * viewMatrix * vec4(vPosition, 1.f);
    vPosition.xz = scaledPos;
    vertColor = vec3(vMorphFactor);
}
