#include "Physics/physics.h"

void Physics::processFrame(std::vector<Body>& bodies) {

    for(auto& b : bodies) {
        bool zeroForce = glm::all(glm::epsilonEqual(b.Force, glm::vec3(0.0f), EPSILON));
        if (zeroForce) continue;

        for (int i = 0; i < bodies.size(); ++i) 
            for(int j = i + 1; j < bodies.size(); ++j) 
                if (areColliding(bodies[i], bodies[j])) {
                    // TODO => Implement collision physics
                }

        applyMotion(b);
    }
}

void Physics::push(Body& sphere, glm::vec3 force) {
    sphere.Force += force;
}

bool Physics::areColliding(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 d = sphereOne.Position - sphereTwo.Position;
    double sqDistance = glm::dot(d, d);

    double tRad = sphereOne.sphere.geometry.getRadius() + sphereTwo.sphere.geometry.getRadius();
    double tRadSq = tRad * tRad;

    return sqDistance <= tRadSq + EPSILON;
}

void Physics::applyMotion(Body& sphere) {
    glm::vec3 pos = sphere.Position;
}

double Physics::getDistance(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 d = sphereOne.Position - sphereTwo.Position;
    double sqDistance = glm::dot(d, d);

    return sqrt(sqDistance);
}

double Physics::gravForce(Body& sphereOne, Body& sphereTwo) {
    double force, distance;
    distance = getDistance(sphereOne, sphereTwo);
    force = (GRAV_CONST * sphereOne.mass * sphereTwo.mass) / (distance*distance);
    return force;
}