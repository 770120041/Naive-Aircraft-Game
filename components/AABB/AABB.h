//
// Created by Haotian on 17/6/16.
//

#ifndef MEGAPROJECT_AABB_H
#define MEGAPROJECT_AABB_H

#include"../../libs/myGL.h"

class AABB {
public:
    AABB();

    ~AABB();

    glm::vec3 min, max;
    glm::vec3 center, radius;

    void getAABB(std::vector<glm::vec3> vertices, GLfloat scale);

    bool isCollided(AABB *a);

    void setAABB(glm::vec3 getmin, glm::vec3 getmax);

    void updataAABB(glm::vec3 getcenter);
};

#endif //MEGAPROJECT_AABB_H
