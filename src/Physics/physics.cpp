#include "Physics/physics.h"

void Physics::processFrame(std::vector<Body>& bodies) {

    for (int i = 0; i < bodies.size(); ++i) {

        for (int j = i + 1; j < bodies.size(); ++j) {
            if (areColliding(bodies[i], bodies[j])) {
                processCollision(bodies[i], bodies[j]);
            }
        }

        bool zeroForce = glm::all(glm::epsilonEqual(bodies[i].Force, glm::vec3(0.0f), (float)EPSILON));
        if (zeroForce) continue;

        applyMotion(bodies[i]);
    }
}

void Physics::init() {

    // TODO => Implement an init function.

    return;
}

void Physics::push(Body& sphere, glm::vec3 force) {
    sphere.Force += force;
}

bool Physics::areColliding(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 d = sphereOne.Position - sphereTwo.Position;
    double sqDistance = glm::dot(d, d);

    double aRad = sphereOne.sphere.geometry.getRadius();
    double bRad = sphereTwo.sphere.geometry.getRadius();

    double tRad = aRad + bRad;
    double tRadSq = tRad * tRad;

    return sqDistance <= tRadSq + EPSILON;
}

void Physics::processCollision(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 collisionNormal = glm::normalize(sphereTwo.Position - sphereOne.Position);

    float oneCollisionVel = glm::dot(sphereOne.Velocity, collisionNormal);
    float twoCollisionVel = glm::dot(sphereTwo.Position, collisionNormal);

    float newOneVel = 
        (oneCollisionVel * 
            ((sphereOne.mass - sphereTwo.mass) / (sphereOne.mass + sphereTwo.mass))) + 
        (twoCollisionVel * 
            ((2 * sphereTwo.mass) / (sphereOne.mass + sphereTwo.mass)));

    float newTwoVel = 
        (oneCollisionVel * 
            ((2 * sphereOne.mass) / (sphereOne.mass + sphereTwo.mass))) + 
        (twoCollisionVel * 
            ((sphereTwo.mass - sphereOne.mass) / (sphereOne.mass + sphereTwo.mass)));

    glm::vec3 impulseOne = collisionNormal * (newOneVel - oneCollisionVel);
    glm::vec3 impulseTwo = collisionNormal * (newTwoVel - twoCollisionVel);

    sphereOne.Velocity += impulseOne;
    sphereTwo.Velocity += impulseTwo;
}

void Physics::applyMotion(Body& sphere) {
    glm::vec3 pos = sphere.Position;
    glm::vec3 force = sphere.Force;
    glm::vec3 vel = sphere.Velocity;
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

void Physics::cleanup() {
    // TODO => Implement a cleanup function

    return;
}