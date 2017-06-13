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

struct MyGLMaterial {
    void clear() {
        materialName = "";
        Ns = d = Tr = 0.f;
        Tf = glm::vec3(1.f);
        illum = 0;
        Ka = Kd = Ks = glm::vec3(0.f);
    }
    std::string materialName;
    GLfloat Ns, d, Tr;
    glm::vec3 Tf;
    GLint illum;
    glm::vec3 Ka, Kd, Ks;
};

class myGL {
public:
    static std::string readShader(const char *filename);

    static bool loadObj(const char *path, const char *fileName, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec3> &out_uvs,
                        std::vector<glm::vec3> &out_normals, std::vector<MyGLMaterial> &out_materials, std::vector<GLfloat> &out_material_ids);

    static bool loadMaterial(const char *path, const char *fileName, std::vector<MyGLMaterial> &out_materials);

    static GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

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
