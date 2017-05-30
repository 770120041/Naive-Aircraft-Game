//
// Created by Haotian on 17/5/30.
//

#include "Aircraft.h"

Aircraft::Aircraft() {

}

Aircraft::~Aircraft() {

}

void Aircraft::setupShaders(const char *vertFile, const char *fragFile) {
    std::string vs, fs;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    vs = myGL::readShader(vertFile);
    fs = myGL::readShader(fragFile);

    const char *vv = vs.c_str();
    const char *ff = fs.c_str();

    glShaderSource(v, 1, &vv, NULL);
    glShaderSource(f, 1, &ff, NULL);

    vs.clear();
    fs.clear();

    glCompileShader(v);
    glCompileShader(f);

    myGL::dumpShaderLog(v);
    myGL::dumpShaderLog(f);

    p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);

    glBindFragDataLocation(p, 0, "FragColor");
    glLinkProgram(p);
    myGL::dumpProgramLog(p);

    vertexLoc = glGetAttribLocation(p, "vertPosition");
    normalLoc = glGetAttribLocation(p, "vertNormal");


    MVPMatrixLoc = glGetUniformLocation(p, "MVPMatrix");
    AmbientLoc = glGetUniformLocation(p, "Ambient");
    LightColorLoc = glGetUniformLocation(p, "LightColor");
    LightDirectionLoc = glGetUniformLocation(p, "LightDirection");
    HalfVectorLoc = glGetUniformLocation(p, "HalfVector");
    ShininessLoc = glGetUniformLocation(p, "Shininess");
    StrengthLoc = glGetUniformLocation(p, "Strength");
}

void Aircraft::updateMVP() {
    MVPMatObj = projMatObj * viewMatObj;
}

void Aircraft::changeSize(int w, int h) {
    if (h == 0) { h = 1; }
    glViewport(0, 0, w, h);

    projMatObj = glm::perspective(glm::radians(45.0f), (float) w / h, 1.0f, 5000.0f);
    updateMVP();
}

void Aircraft::setupBuffers(const char *objFile) {
    myGL::loadObj(objFile, vertices, uvs, normals);

    GLuint buffers[2];
    glGenVertexArrays(3, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(2, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);

    GLuint ebo[5];
    static const GLushort vertex_indices[] = {0, 1, 2};
    glGenBuffers(2, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
}

void Aircraft::setUniforms() {
    glUniformMatrix4fv(MVPMatrixLoc, 1, 0, &MVPMatObj[0][0]);

    // light param
    glUniform3fv(AmbientLoc, 1, &Ambient[0]);
    glUniform3fv(LightColorLoc, 1, &LightColor[0]);
    glUniform3fv(LightDirectionLoc, 1, &LightDirection[0]);
    glUniform3fv(HalfVectorLoc, 1, &HalfVector[0]);
    glUniform3fv(ShininessLoc, 1, &Shininess);
    glUniform3fv(StrengthLoc, 1, &Strength);
}

void Aircraft::setCameraCoordinate() {
    glm::vec3 dir = viewDirVect * polar_r;
    glm::vec3 fuse = glm::vec3(1, 1, 1);

    LightDirection = HalfVector = viewDirVect;

    viewMatObj = glm::lookAt(
            dir,
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
    );

    updateMVP();
}

void Aircraft::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p);
    setUniforms();

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glutSwapBuffers();
}

void Aircraft::processSpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(1.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case GLUT_KEY_DOWN:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(-1.0f), glm::vec3(1.0, 0.0, 0.0));
            break;
        case GLUT_KEY_LEFT:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
            break;
        case GLUT_KEY_RIGHT:
            viewDirVect = glm::rotate(viewDirVect, glm::radians(-1.0f), glm::vec3(0.0, 1.0, 0.0));
            break;
        default:
            break;
    }
    setCameraCoordinate();
}

void Aircraft::processNormalKeys(unsigned char key, int x, int y) {
    if (key == 27) {
        glDeleteVertexArrays(3, vao);
        glDeleteProgram(p);
        glDeleteShader(v);
        glDeleteShader(f);
        exit(0);
    }

    switch (key) {
        case 'w':
        case 'W':
            polar_r *= 1.05;
            break;
        case 's':
        case 'S':
            polar_r /= 1.05;
            break;
        default:
            break;
    }

    setCameraCoordinate();
}
