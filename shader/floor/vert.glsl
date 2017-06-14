#version 410

void main() {
    const vec4 vertices[] = vec4[](
        vec4(-0.25f, -0.25f, 0.5f, 1.f),
        vec4(0.25f, -0.25f, 0.5f, 1.f),
        vec4(0.25f, 0.25f, 0.5f, 1.f),
        vec4(-0.25f, 0.25f, 0.5f, 1.f)
    );

    gl_Position = vertices[gl_VertexID];

}
