#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "body.h"

inline constexpr float dt = 1.0 / 60.0;
inline constexpr double GRAV_CONST = 6.67430e-11;
inline constexpr double EPSILON = 1e-6;

inline double frameCount = 0;

class Physics {
public:
    Physics();

    void push(Body& sphere, glm::vec3 force);
    void processFrame(std::vector<Body>& bodies);
    bool shouldClose();
    void cleanup();

private:
    float Speed;
    float Mass;
    float Radius;
    bool endSim;
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Acceleration;

    bool areColliding(Body& sphereOne, Body& sphereTwo);
    void processCollision(Body& sphereOne, Body& sphereTwo);
    double getDistance(Body& sphereOne, Body& sphereTwo);
    double gravForce(Body& sphereOne, Body& sphereTwo);
};

#endif