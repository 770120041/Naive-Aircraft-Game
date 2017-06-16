//
// Created by Haotian on 17/6/16.
//

#include "AABB.h"
#include <iostream>
using namespace std;


AABB::AABB() {
}


AABB::~AABB() {
}

void AABB::getAABB(std::vector <glm::vec3> vertices, GLfloat scale = 1) {
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i][0] > max[0])
            max[0] = vertices[i][0];
        if (vertices[i][1] > max[1])
            max[1] = vertices[i][1];
        if (vertices[i][2] > max[2])
            max[2] = vertices[i][2];
        if (vertices[i][0] < min[0])
            min[0] = vertices[i][0];
        if (vertices[i][1] < min[1])
            min[1] = vertices[i][1];
        if (vertices[i][2] < min[2])
            min[2] = vertices[i][2];
    }
    min *= scale;
    max *= scale;
    center = (min + max) / 2;
    radius = max - center;
}

void AABB::setAABB(glm::vec3 getmin, glm::vec3 getmax) {
    min = getmin;
    max = getmax;
    center = (min + max) / 2;
    radius = max - center;
}

bool AABB::isCollided(AABB *a) {
    if (max[0] < a->min[0] || min[0] > a->max[0]) return false;
    if (max[1] < a->min[1] || min[1] > a->max[1]) return false;
    if (max[2] < a->min[2] || min[2] > a->max[2]) return false;
    return true;
}

void AABB::updataAABB(glm::vec3 getcenter) {
    max += center - getcenter;
    min += center - getcenter;
    center = getcenter;
    radius = max - center;
}