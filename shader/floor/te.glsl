#version 410

   // #define p(i)  gl_in[i].gl_Position
layout (quads, equal_spacing, ccw) in;
#define M_PI		3.14159265358979323846

out vec4 Color;


vec4 p(int i) {
    return gl_in[i].gl_Position;
}

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;


        Color = vec4(0.f);
        vec4 pos = v*(u*p(0) + (1-u)*p(1)) + (1-v)*(u*p(3) + (1-u)*p(2));

        gl_Position = pos;
}
