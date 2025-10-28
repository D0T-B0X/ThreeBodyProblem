#include "Physics/physics.h"

Physics::Physics() : Speed(3.0f), endSim(false) {
    dt = 1.0 / 60.0;
}

Physics::Physics(float speed) : Speed(speed), endSim(false) {
    dt = 1.0 / 60.0;
}

Physics::Physics(float timeStep, float speed) : Speed(speed), endSim(false) {
    dt = timeStep;
}

void Physics::processFrame(std::vector<Body*> bodies) {

    for (int i = 0; i < bodies.size(); ++i) {

        Body* body = bodies[i];

        for (int j = i + 1; j < bodies.size(); ++j) {
            Body* colBody = bodies[j];
            if (colBody == body) continue;

            if (areColliding(*colBody, *body)) {
                processCollision(*colBody, *body);
            }
        }

        // Euler integration to update velocity vector
        body->Velocity += body->Acceleration * dt;

        // Euler integration to update position vector
        body->Position += body->Velocity * dt * Speed;
        
        // Natural exponential velocity decay: v(t) = v₀ * e^(-λt)
        // λ (lambda) controls decay rate: higher = faster decay
        if (!isZero(body->Velocity)) {
            float vLambda = 0.5f;  // Adjust this for desired decay speed (0.1 = slow, 1.0 = fast)
            float vDecayFactor = glm::exp(-vLambda * dt);
            body->Velocity *= vDecayFactor;
        }

        if (!isZero(body->Acceleration)) {
            float aLambda = 0.9f;
            float aDecayFactor(glm::exp(-aLambda * dt));
            body->Acceleration *= aDecayFactor;
        }

        // End simulation if particle crosses the boundary
        if (body->Position.x >= 20.0f) {
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
    sphere.Velocity += sphere.Force / sphere.Mass;
}

bool Physics::isZero(glm::vec3& vector) {
    if (vector == glm::vec3(0)) return true;

    bool zero = glm::all(glm::epsilonEqual(vector, glm::vec3(0), glm::vec3(EPSILON)));
    if (zero) {
        vector = glm::vec3(0);
    }

    return zero;
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

    glm::vec3 velOne = (((sphereOne.Mass - sphereTwo.Mass) * sphereOne.Velocity) + ((sphereTwo.Mass + sphereTwo.Mass) * sphereTwo.Velocity)) / (sphereOne.Mass + sphereTwo.Mass);
    glm::vec3 velTwo = (((sphereOne.Mass + sphereTwo.Mass) * sphereOne.Velocity) + ((sphereTwo.Mass - sphereOne.Mass) * sphereTwo.Velocity)) / (sphereOne.Mass + sphereTwo.Mass);

    sphereOne.Velocity = velOne;
    sphereTwo.Velocity = velTwo;
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