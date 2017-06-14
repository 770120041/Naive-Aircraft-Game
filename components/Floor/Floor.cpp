//
// Created by Haotian on 17/6/14.
//

#include "Floor.h"

Floor::Floor() {

}

void Floor::setupShaders(const char *vertFile, const char *tcFile, const char *teFile, const char *fragFile) {
    GLuint vi, tci, tei, fi;

    vi = myGL::loadShader(GL_VERTEX_SHADER, vertFile);
    tci = myGL::loadShader(GL_TESS_CONTROL_SHADER, tcFile);
    tei = myGL::loadShader(GL_TESS_EVALUATION_SHADER, teFile);
    fi = myGL::loadShader(GL_FRAGMENT_SHADER, fragFile);

    program = glCreateProgram();
    glAttachShader(program, vi);
    glAttachShader(program, tci);
    glAttachShader(program, tei);
    glAttachShader(program, fi);

    glBindFragDataLocation(program, 0, "FragColor");
    glLinkProgram(program);
    myGL::dumpProgramLog(program);

    glDeleteShader(vi);
    glDeleteShader(tci);
    glDeleteShader(tei);
    glDeleteShader(fi);
}

void Floor::render() {
    glUseProgram(program);
    glDrawArrays(GL_PATCHES, 0, 3);
    glUseProgram(0);
}
