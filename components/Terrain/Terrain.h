//
// Created by Haotian on 17/6/14.
//

#ifndef MEGAPROJECT_TERRAIN_H
#define MEGAPROJECT_TERRAIN_H

#include "../../libs/PerlinNoise.h"
#include "../../libs/myGL.h"

#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

class Terrain {
public:
    Terrain(glm::mat4 &, glm::mat4 &, glm::vec3 &);
    void setupShaders(const char *vertFile, const char *fragFile);
    void setupBuffers();
    void render();

private:
    void noise();
    void setupPlane();
    void createTile(glm::vec2 offset, GLfloat scale, GLint edgeMorph);

    struct {
        int NONE = 0;
        int TOP = 1;
        int LEFT = 2;
        int BOTTOM = 4;
        int RIGHT = 8;
    } Edge;

    struct {
        GLint ViewMatrixLoc, ProjectionMatrixLoc;
        GLint uEdgeMorphLoc, globalOffsetLoc, uOffsetLoc, uScaleLoc;
    } BodyUniformLoc;

    struct TileInfo {
        TileInfo (glm::vec2 offset, GLfloat scale, GLfloat edgeMorph) : offset(offset), scale(scale), edgeMorph(edgeMorph) {}
        glm::vec2 offset;
        GLfloat scale;
        GLint edgeMorph;
    };
    glm::mat4 &viewMatObj, &projMatObj;
    GLuint program, vao, tbo, vbo;
    GLfloat *noiseData = nullptr;
    vector<glm::vec2> plane;
    glm::vec3 &globalOffset;
    vector<TileInfo> tiles;
    int levels = 10;
    int worldWidth = 3000;
};


#endif //MEGAPROJECT_TERRAIN_H
