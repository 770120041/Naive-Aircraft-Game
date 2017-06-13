//
// Created by Haotian on 17/6/13.
//

#ifndef MEGAPROJECT_SKYBOX_H
#define MEGAPROJECT_SKYBOX_H

#include "../libs/myGLHeaders.h"
#include <string>
#include <iostream>
#include <vector>
#include <SOIL.h>

class SkyBox {
public:
    static GLuint loadCubemap(std::vector<std::string> faces);
    static void render_skybox(GLuint skyboxVAO, GLuint cubemapTexture);
    static void skybox_buffer(GLuint &skyboxVAO, GLuint &skyboxVBO);
};


#endif //MEGAPROJECT_SKYBOX_H
