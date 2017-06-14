//
// Created by Haotian on 17/5/30.
//

#include "myGL.h"

#include <iostream>

std::string myGL::readShader(const char *filename) {
    std::ifstream in(filename);
    std::stringstream sStr;
    sStr << in.rdbuf();
    return sStr.str();
}

GLuint myGL::loadShader(GLuint SHADER_TYPE_MACRO, const char *filename) {
    GLuint shader;
    std::string ss;
    const char *ff;
    shader = glCreateShader(SHADER_TYPE_MACRO);
    ss = myGL::readShader(filename);
    ff = ss.c_str();
    glShaderSource(shader, 1, &ff, nullptr);
    glCompileShader(shader);
    myGL::dumpShaderLog(shader);
    return shader;
}

bool myGL::loadMaterial(const char *path, const char *fileName, std::vector<MyGLMaterial> &out_materials) {
    MyGLMaterial temp;
    bool hasMaterial = false;
    char tempName[200];
    std::string filePath(path);
    filePath += fileName;

    FILE *file = fopen(filePath.c_str(), "r");
    if (file == NULL) {
        printf("Failed to open the file in `%s`!\n", filePath.c_str());
        return false;
    }

    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) { break; }

        if (strcmp(lineHeader, "newmtl") == 0) {
            if (!hasMaterial) {
                hasMaterial = true;
            } else {
                out_materials.push_back(temp);
            }
            temp.clear();
            fscanf(file, "%s\n", tempName);
            temp.materialName = tempName;
        } else if (strcmp(lineHeader, "Ns") == 0) {
            fscanf(file, "%f\n", &temp.Ns);
        } else if (strcmp(lineHeader, "d") == 0) {
            fscanf(file, "%f\n", &temp.d);
        } else if (strcmp(lineHeader, "Tr") == 0) {
            fscanf(file, "%f\n", &temp.Tr);
        } else if (strcmp(lineHeader, "Tf") == 0) {
            fscanf(file, "%f %f %f\n", &temp.Tf.x, &temp.Tf.y, &temp.Tf.z);
        } else if (strcmp(lineHeader, "illum") == 0) {
            fscanf(file, "%d\n", &temp.illum);
        } else if (strcmp(lineHeader, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &temp.Ka.x, &temp.Ka.y, &temp.Ka.z);
        } else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &temp.Kd.x, &temp.Kd.y, &temp.Kd.z);
        } else if (strcmp(lineHeader, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &temp.Ks.x, &temp.Ks.y, &temp.Ks.z);
        } else {
            while (fgetc(file) != '\n');
        }
    }

    if (hasMaterial) { out_materials.push_back(temp); }
    return true;
}

bool myGL::loadObj(const char *path, const char *fileName, std::vector<glm::vec3> &out_vertices,
                   std::vector<glm::vec3> &out_uvs,
                   std::vector<glm::vec3> &out_normals, std::vector<MyGLMaterial> &out_materials,
                   std::vector<GLfloat> &out_material_ids) {
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::string filePath(path);
    filePath += fileName;

    GLfloat currentMaterialId = 0;

    FILE *file = fopen(filePath.c_str(), "r");
    if (file == NULL) {
        printf("Failed to open the file in `%s`!\n", filePath.c_str());
        return false;
    }

    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) { break; }

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec3 uv;
            fscanf(file, "%f %f %f\n", &uv.x, &uv.y, &uv.z);
            temp_uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
            char buff[255];
            fgets(buff, 250, file);
            int matches = sscanf(buff, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
            if (matches != 12) {
                matches = sscanf(buff, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
                                 &normalIndex[2]);
                if (matches != 9) {
                    fprintf(stderr, "This is not standard obj our program can process!");
                    return false;
                } else {
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    uvIndices.push_back(uvIndex[0]);
                    uvIndices.push_back(uvIndex[1]);
                    uvIndices.push_back(uvIndex[2]);
                    normalIndices.push_back(normalIndex[0]);
                    normalIndices.push_back(normalIndex[1]);
                    normalIndices.push_back(normalIndex[2]);
                    for (int i = 0; i < 3; i++) {
                        out_material_ids.push_back(currentMaterialId);
                    }
                }
            } else {
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);

                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[2]);
                vertexIndices.push_back(vertexIndex[3]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[2]);
                uvIndices.push_back(uvIndex[3]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[2]);
                normalIndices.push_back(normalIndex[3]);

                for (int i = 0; i < 6; i++) {
                    out_material_ids.push_back(currentMaterialId);
                }

            }
        } else if (strcmp(lineHeader, "mtllib") == 0) {
            char mtlFile[200];
            fscanf(file, "%s\n", mtlFile);
            loadMaterial(path, mtlFile, out_materials);
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            char mtl[200];
            fscanf(file, "%s\n", mtl);
            bool flag = true;
            for (int i = 0; i < out_materials.size(); i++) {
                if (mtl == out_materials[i].materialName) {
                    currentMaterialId = i;
                    flag = false;
                    break;
                }
            }
            if (flag) { exit(-1); }
        } else {
            while (fgetc(file) != '\n');
        }
    }

    // last vertex
    //out_material_mapper.push_back(vertexIndices.size());

    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];
        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }
    return true;
}

void myGL::dumpShaderLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char *) malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

void myGL::dumpProgramLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char *) malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

