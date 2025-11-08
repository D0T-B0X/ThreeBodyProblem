#ifndef BODY_H
#define BODY_H

#include <string>
#include "Renderer/mesh.h"

struct Body {
    Sphere sphere;

    float Mass = 1.0f;
    glm::vec3 Position;
    glm::vec3 Velocity = glm::vec3(0);
    glm::vec3 Acceleration = glm::vec3(0);
    glm::vec3 Force = glm::vec3(0);
    glm::vec3 vForceAccumulator = glm::vec3(0);

    void setRadius(float radius) {
        sphere.setRadius(radius);
    }
};

#endif