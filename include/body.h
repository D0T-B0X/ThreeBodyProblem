#ifndef BODY_H
#define BODY_H

#include "Renderer/mesh.h"

struct Body {
    Sphere sphere;

    float mass;
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Accelaration;
    glm::vec3 Force;
};

#endif