//
// Created by Haotian on 17/5/30.
//

#ifndef MEGAPROJECT_AIRCRAFT_H
#define MEGAPROJECT_AIRCRAFT_H

#include "../libs/myGL.h"

#include <iostream>
using namespace std;

const glm::vec3 X = glm::vec3(1.0f, 0.f, 0.f);
const glm::vec3 Y = glm::vec3(0.0f, 1.f, 0.f);
const glm::vec3 Z = glm::vec3(0.0f, 0.f, 1.f);

const GLuint DEPTH_TEXTURE_SIZE = 2048;

class Aircraft {
public:
    Aircraft();

    ~Aircraft();

    void loadIdentity() { setCameraCoordinate(); }

    void setupShaders(const char *vertBodyFile, const char *fragBodyFile, const char *vertShadowFile, const char *fragShadowFile);

    void setupBuffers(const char *objFile);

    void processSpecialKeys(int key, int x, int y);

    void processNormalKeys(unsigned char key, int x, int y);

    void changeSize(int w, int h);

    void render();

    void setupShadowMat();

protected:
    void updateMVP();

    void setBodyUniforms();

    void setShadowUniforms();

    void setCameraCoordinate();

private:
    GLuint pb, vb, fb, shadowProgram, shadowVert, shadowFrag;

    struct BodyUniformLocStruct {
        GLint vertexLoc, normalLoc;
        GLint ModelMatrixLoc, ViewMatrixLoc, ProjectionMatrixLoc;
        GLint MVPMatrixLoc, AmbientLoc, LightColorLoc, LightDirectionLoc, HalfVectorLoc, ShininessLoc, StrengthLoc;
    } BodyUniformLoc, ShadowUniformLoc;

    glm::vec3 Ambient = glm::vec3(0.4f), LightColor = glm::vec3(1.0f), LightDirection, HalfVector;
    GLfloat Shininess = 1.0f, Strength = 10.0f;

    GLuint vao[3];

    GLfloat WinRatio;

    GLuint depth_texture, depth_fbo;

    GLint winW, winH;

    glm::vec3 viewDirVect = X;

    glm::mat4 modelMatObj = glm::mat4(1.f), viewMatObj, projMatObj, MVPMatObj, shadowMVPMatObj;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.

    GLfloat polar_r = 2000;
};

#endif //MEGAPROJECT_AIRCRAFT_H
