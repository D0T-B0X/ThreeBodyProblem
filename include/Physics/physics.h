#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "body.h"

inline constexpr float dt = 0.016f;
inline constexpr double GRAV_CONST = 6.67430e-11;
inline constexpr float EPSILON = 1e-6f;

class Physics {
public:
    Physics() { }

    void init();
    void push(Body& sphere, glm::vec3 force);
    void processFrame(std::vector<Body>& bodies);
    void cleanup();

private:
    float Mass;
    float Radius;
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Acceleration;

    bool areColliding(Body& sphereOne, Body& sphereTwo);
    void processCollision(Body& sphereOne, Body& sphereTwo);
    void applyMotion(Body& sphere);
    double getDistance(Body& sphereOne, Body& sphereTwo);
    double gravForce(Body& sphereOne, Body& sphereTwo);
};

#endif