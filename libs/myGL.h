//
// Created by Haotian on 17/5/30.
//

#ifndef MEGAPROJECT_MYGLFILE_H
#define MEGAPROJECT_MYGLFILE_H

#include "myGLHeaders.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class myGL {
public:
    static std::string readShader(const char *filename);

    static bool loadObj(const char *path, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_uvs,
                        std::vector<glm::vec3> &out_normals);

    static void dumpShaderLog(GLuint obj);

    static void dumpProgramLog(GLuint obj);

    static void dumpGLErrorLog() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("glError: %s\n", gluErrorString(err));
        }
    }
};


#endif //MEGAPROJECT_MYGLFILE_H
