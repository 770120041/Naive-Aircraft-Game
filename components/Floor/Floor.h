//
// Created by Haotian on 17/6/14.
//

#ifndef MEGAPROJECT_FLOOR_H
#define MEGAPROJECT_FLOOR_H

#include "../../libs/myGL.h"

class Floor {
public:
    Floor();
    void setupShaders(const char *vertFile, const char *tcFile, const char *teFile, const char *fragFile);
    void render();

private:
    GLuint program;
};


#endif //MEGAPROJECT_FLOOR_H
