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
        body->Force = glm::vec3(0);

        if (body->sphere.mesh.source) continue;

        // Calculate gravitational forces between this body and all later bodies
        for(int j = i + 1; j < bodies.size(); ++j) {
            Body* sBody = bodies[j];
            // Skip if the other body is a light source
            if (sBody->sphere.mesh.source) continue;

            calculateGravForce(*body, *sBody);
        }

        calculateForce(*body);
        updateState(*body);

        if (onSurface(*body))
            processSurfaceCollision(*body);

        for (int j = i + 1; j < bodies.size(); ++j) {
            Body* colBody = bodies[j];
            if (colBody->sphere.mesh.source == true) continue;

            if (areColliding(*colBody, *body) && !((isZero(body->Velocity) && isZero(colBody->Velocity)))) {
                // endSim = true;
                
                processCollision(*colBody, *body);
            }
        }
        
        // Natural exponential velocity decay: v(t) = v₀ * e^(-λt)
        // λ (lambda) controls decay rate: higher = faster decay
        if (!isZero(body->Velocity)) {
            float vLambda = 0.0f;  // Adjust this for desired decay speed (0.1 = slow, 1.0 = fast)
            float vDecayFactor = glm::exp(-vLambda * dt);
            body->Velocity *= vDecayFactor;
        }
    }
}

void Physics::wait(float sec) {
}

bool Physics::shouldClose() {
    return endSim;
}

void Physics::push(Body& sphere, glm::vec3 impulse) {
    sphere.Velocity += impulse;
}

bool Physics::isZero(glm::vec3& vector) {
    if (vector == glm::vec3(0)) return true;

    bool zero = glm::all(glm::epsilonEqual(vector, glm::vec3(0), glm::vec3(EPSILON)));
    
    return zero;
}

void Physics::updateState(Body& body) {
    // get the acceleration vector from the total force on the body
    body.Acceleration = body.Force / body.Mass; 

    // Euler integration to update vecloty vector
    body.Velocity += body.Acceleration * dt;

    // Euler integration to update position vector
    body.Position += body.Velocity * dt;
}

float Physics::calculateDistanceSquare(Body& sphereOne, Body& sphereTwo) {
    glm::vec3 vDistance = sphereTwo.Position - sphereOne.Position;
    float fDistSq = glm::dot(vDistance, vDistance);
    return fDistSq;
}

void Physics::calculateGravForce(Body& sphereOne, Body& sphereTwo) {
    float fDistanceSq = calculateDistanceSquare(sphereOne, sphereTwo);
    
    // Clamp distance to prevent infinite forces when bodies are too close
    float minDistSq = 1.0f;  // Minimum distance squared (1.0 unit²)
    if (fDistanceSq < minDistSq + EPSILON) return;
    
    // Direction FROM sphereOne TO sphereTwo (attraction direction)
    glm::vec3 vDirOne = glm::normalize(sphereTwo.Position - sphereOne.Position);
    glm::vec3 vDirTwo = -vDirOne;  // Opposite direction for sphereTwo

    // Use MUCH smaller gravitational constant to prevent runaway acceleration
    // The Speed multiplier (3.0x) amplifies motion, so G must be smaller
    float gravForce = GRAV_CONST * ((sphereOne.Mass * sphereTwo.Mass) / fDistanceSq);

    sphereOne.vForceAccumulator += gravForce * vDirOne;
    sphereTwo.vForceAccumulator += gravForce * vDirTwo;
}

void Physics::calculateForce(Body& body) {
    glm::vec3 vGravForce = body.Mass * GRAV_FORCE;
    glm::vec3 vVelocityNormal = glm::normalize(body.Velocity);
    glm::vec3 vForceAccumulator = body.vForceAccumulator;

    body.Force = vGravForce + vForceAccumulator;
    body.vForceAccumulator = glm::vec3(0);
}

bool Physics::onSurface(Body& body) {
    float rad = body.sphere.geometry.getRadius();
    float y = body.Position.y;
    float surfaceY = -2.0f;

    return y - rad <= surfaceY + EPSILON;
}

void Physics::processSurfaceCollision(Body& body) {
    // Apply coefficient of restitution (energy loss) and REVERSE direction
    body.Velocity.y = body.Velocity.y * -0.8f;
    // Clamp position to surface to prevent sinking
    float rad = body.sphere.geometry.getRadius();
    float surfaceY = -2.0f;
    body.Position.y = surfaceY + rad;
    
    // Stop micro-bouncing: if velocity is too small, set to zero (resting state)
    if (glm::abs(body.Velocity.y) < 0.1f) {
        body.Velocity.y = 0.0f;
    }
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
    // Calculate collision normal (direction from one to two)
    glm::vec3 collisionNormal = glm::normalize(sphereTwo.Position - sphereOne.Position);
    
    // Calculate overlap distance
    float distance = glm::length(sphereTwo.Position - sphereOne.Position);
    float radiusSum = sphereOne.sphere.geometry.getRadius() + sphereTwo.sphere.geometry.getRadius();
    float overlap = radiusSum - distance;
    
    // Position correction: push spheres apart by half the overlap each
    // This prevents them from staying stuck together
    if (overlap > 0) {
        glm::vec3 correction = collisionNormal * (overlap / 2.0f);
        sphereOne.Position -= correction;  // Push sphere one away
        sphereTwo.Position += correction;  // Push sphere two away
    }
    
    // Calculate new velocities using elastic collision formula
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

void Physics::cleanup() {
    // TODO => Implement a cleanup function

    return;
}