//
// Created by Haotian on 17/6/14.
//

#include "Floor.h"

Floor::Floor() {

}

void Floor::setupShaders(const char *vertFile, const char *tcFile, const char *teFile, const char *fragFile) {
    program = glCreateProgram();

    myGL::attachShader(program, GL_VERTEX_SHADER, vertFile);
    myGL::attachShader(program, GL_TESS_CONTROL_SHADER, tcFile);
    myGL::attachShader(program, GL_TESS_EVALUATION_SHADER, teFile);
    myGL::attachShader(program, GL_FRAGMENT_SHADER, fragFile);

    glBindFragDataLocation(program, 0, "FragColor");
    glLinkProgram(program);
    myGL::dumpProgramLog(program);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glGenVertexArrays(1, &vao);
}

void Floor::render() {
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawArrays(GL_PATCHES, 0, 4);
    glUseProgram(0);
}
