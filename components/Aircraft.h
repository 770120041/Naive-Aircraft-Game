//
// Created by Haotian on 17/5/30.
//

#ifndef MEGAPROJECT_AIRCRAFT_H
#define MEGAPROJECT_AIRCRAFT_H

#include "../libs/myGL.h"

class Aircraft {
public:
    Aircraft();

    ~Aircraft();

    void loadIdentity() { setCameraCoordinate(); }

    void setupShaders(const char *vertFile, const char *fragFile);

    void setupBuffers(const char *objFile);

    void processSpecialKeys(int key, int x, int y);

    void processNormalKeys(unsigned char key, int x, int y);

    void changeSize(int w, int h);

    void render();

protected:
    void updateMVP();

    void setUniforms();

    void setCameraCoordinate();

private:
    GLuint p, v, f;

    GLint vertexLoc, normalLoc;

    GLint MVPMatrixLoc, AmbientLoc, LightColorLoc, LightDirectionLoc, HalfVectorLoc, ShininessLoc, StrengthLoc;

    glm::vec3 Ambient = glm::vec3(0.8f, 0.8f, 0.8f), LightColor = glm::vec3(1.0f, 1.0f,
                                                                            1.0f), LightDirection, HalfVector;
    GLfloat Shininess = 20.0f, Strength = 10.0f;

    GLuint vao[3];

    glm::vec3 viewDirVect = glm::vec3(1.0f, 0.f, 0.f);

    glm::mat4 viewMatObj, projMatObj, MVPMatObj;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.

    GLfloat polar_r = 1500;
};

#endif //MEGAPROJECT_AIRCRAFT_H
