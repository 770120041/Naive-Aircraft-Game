#version 410 core

uniform int worldWidth;
uniform vec3 globalOffset;
uniform float uScale;
uniform sampler2D uHeightData;
uniform sampler2D uRock;

layout(origin_upper_left) in vec4 gl_FragCoord;
in vec3 vPosition;
in vec2 vPosXY;
in vec3 vertColor;
out vec4 FragColor;

void main() {
  // Base color
  vec3 light = vec3(0.0, 850.0, -50.0);

  vec3 color = texture(uRock, vPosition.xz * worldWidth / uScale).rgb;

  // Add height fog
  float fogFactor = clamp( 1.0 - vPosition.y / 40000, 0.0, 1.0 );
  fogFactor = 0.96 * pow( fogFactor, 5.4 );
  float fogAngle = dot( normalize( - vPosition ), normalize( vPosition - light ) );
  fogAngle = smoothstep( 0.0, 1.0, fogAngle );
  vec3 fogColor = vec3( 0.86, 0.95, 1.0 );
  color = mix( color, fogColor, fogFactor );

  // Add distance fog
//  float depth = gl_FragCoord.z / gl_FragCoord.w;
//  fogFactor = smoothstep( 300.0, 1000.0, depth );
//  fogColor = mix( vec3( 1.0, 0.945, 0.847 ), vec3( 0.98, 0.77, 0.33), fogAngle );
//  color = mix( color, fogColor, fogFactor );

    FragColor = vec4(vec3(color), 1.f);
}