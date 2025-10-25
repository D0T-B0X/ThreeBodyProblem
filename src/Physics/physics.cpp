#include "Physics/physics.h"

Physics::Physics() {

    // TODO => Implement an init function.
    endSim = false;
    Speed = 3.0f;

}

void Physics::processFrame(std::vector<Body>& bodies) {

    for (auto& body : bodies) {

        // Euler integration to update position vector
        body.Position += body.Velocity * dt * Speed;
        
        // Natural exponential velocity decay: v(t) = v₀ * e^(-λt)
        // λ (lambda) controls decay rate: higher = faster decay
        float lambda = 0.4f;  // Adjust this for desired decay speed (0.1 = slow, 1.0 = fast)
        float decayFactor = glm::exp(-lambda * dt);
        body.Velocity *= decayFactor;

        // End simulation if particle crosses the boundary
        if (body.Position.x >= 20.0f) {
            endSim = true;
            break;
        }

    }
}

bool Physics::shouldClose() {
    return endSim;
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
            ((sphereOne.Mass - sphereTwo.Mass) / (sphereOne.Mass + sphereTwo.Mass))) + 
        (twoCollisionVel * 
            ((2 * sphereTwo.Mass) / (sphereOne.Mass + sphereTwo.Mass)));

    float newTwoVel = 
        (oneCollisionVel * 
            ((2 * sphereOne.Mass) / (sphereOne.Mass + sphereTwo.Mass))) + 
        (twoCollisionVel * 
            ((sphereTwo.Mass - sphereOne.Mass) / (sphereOne.Mass + sphereTwo.Mass)));

    glm::vec3 impulseOne = collisionNormal * (newOneVel - oneCollisionVel);
    glm::vec3 impulseTwo = collisionNormal * (newTwoVel - twoCollisionVel);

    sphereOne.Velocity += impulseOne;
    sphereTwo.Velocity += impulseTwo;
}

double Physics::getDistance(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 d = sphereOne.Position - sphereTwo.Position;
    double sqDistance = glm::dot(d, d);

    return sqrt(sqDistance);
}

double Physics::gravForce(Body& sphereOne, Body& sphereTwo) {
    double force, distance;
    distance = getDistance(sphereOne, sphereTwo);
    force = (GRAV_CONST * sphereOne.Mass * sphereTwo.Mass) / (distance*distance);
    return force;
}

void Physics::cleanup() {
    // TODO => Implement a cleanup function

    return;
}