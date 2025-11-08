/**
 * @file physics.h
 * @author DotBox
 * @brief Physics engine for N-body gravitational simulation
 * 
 * This module implements the core physics calculations for the three-body problem simulator.
 * It handles numerical integration using Euler's method, collision detection via sphere overlap
 * testing, and exponential velocity/acceleration damping for realistic motion decay.
 * 
 * The engine uses a fixed timestep accumulator pattern for deterministic simulation,
 * decoupled from rendering. Position, velocity, and acceleration are updated each frame
 * using kinematic equations, with optional gravitational force accumulation between bodies.
 * 
 * Key features:
 * - Euler integration for position/velocity updates
 * - Exponential decay functions for natural motion damping: v(t) = v₀ * e^(-λt)
 * - Sphere-sphere collision detection (distance-based)
 * - Impulse-based collision response (elastic collisions)
 * - Configurable timestep and simulation speed
 * - Boundary-based simulation termination
 * 
 * @version 0.1
 * @date 2025-10-28
 * 
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <future>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "body.h"

// Global physics constants and parameters
inline float dt;                                                      ///< Physics timestep (seconds per frame)
inline constexpr double GRAV_CONST = 6.67430e-11;                     ///< Gravitational constant (N⋅m²/kg²)
inline constexpr glm::vec3 GRAV_FORCE = glm::vec3(0.0f, 0.0f, 0.0f); ///< Earth's gravitational force
inline constexpr double EPSILON = 1e-3;                               ///< Numerical tolerance for zero comparisons

class Physics {
public:

    /**
     * @brief Default constructor for the Physics Engine.
     * 
     * Initializes the physics engine with default parameters:
     * - Speed: 3.0 (simulation speed multiplier)
     * - Timestep: 1/60 seconds (60 FPS fixed timestep)
     * - Simulation state: active (endSim = false)
     */
    Physics();

    /**
     * @brief Construct physics engine with custom simulation speed.
     * 
     * Useful for creating slow-motion or fast-forward effects without
     * changing the timestep or affecting physics accuracy.
     * 
     * @param speed Simulation speed multiplier (1.0 = normal, >1.0 = faster, <1.0 = slower)
     */
    Physics(float speed);

    /**
     * @brief Construct physics engine with custom timestep and speed.
     * 
     * Allows full control over both temporal accuracy and simulation speed.
     * Smaller timesteps improve accuracy but increase computational cost.
     * 
     * @param timeStep Physics update interval in seconds (e.g., 1/120 = 120 FPS physics)
     * @param speed Simulation speed multiplier applied to velocity calculations
     */
    Physics(float timeStep, float speed);

    /**
     * @brief Apply an instantaneous impulse (force) to a body.
     * 
     * Directly modifies the body's velocity based on F = ma, where the force
     * is divided by the body's mass to calculate acceleration, which is then
     * integrated into velocity. This simulates an instantaneous push or collision.
     * 
     * @param sphere The Body object to receive the impulse
     * @param force Force vector in Newtons (direction and magnitude)
     */
    void push(Body& sphere, glm::vec3 force);

    void wait(float sec);

    /**
     * @brief Execute one physics timestep for all bodies in the simulation.
     * 
     * This is the main physics loop that performs:
     * 1. Velocity integration: v += a * dt
     * 2. Position integration: p += v * dt * speed
     * 3. Exponential velocity damping: v *= e^(-λ*dt) (simulates drag/friction)
     * 4. Exponential acceleration damping: a *= e^(-λ*dt) (force decay)
     * 5. Boundary checking: terminate simulation if body crosses threshold
     * 
     * Uses Euler integration for simplicity. Future versions may implement
     * RK4 or Verlet integration for improved numerical stability.
     * 
     * @param bodies Reference to vector of all Body objects in the simulation
     */
    void processFrame(std::vector<Body*> bodies);

    /**
     * @brief Check if the simulation should terminate.
     * 
     * Returns true when any body crosses the defined boundary threshold
     * or when another termination condition is met.
     * 
     * @return true if simulation should stop, false otherwise
     */
    bool shouldClose();

    /**
     * @brief Clean up physics engine resources.
     * 
     * Currently a placeholder for future cleanup operations
     * (e.g., memory deallocation, file writing, logging).
     */
    void cleanup();

private:
    // Simulation parameters
    float Speed;              ///< Global speed multiplier for all motion
    bool endSim;              ///< Flag to terminate simulation when boundary reached

    /**
     * @brief Check if a vector is approximately zero within epsilon tolerance.
     * 
     * Uses component-wise comparison with EPSILON threshold to determine if
     * a vector is effectively zero. Useful for preventing unnecessary calculations
     * on negligible values and for stopping decay operations.
     * 
     * @param vector The 3D vector to test
     * @return true if all components are within EPSILON of zero, false otherwise
     */
    bool isZero(glm::vec3& vector);

    void updateState(Body& body);

    float calculateDistanceSquare(Body& sphereOne, Body& sphereTwo);

    void calculateGravForce(Body& sphereOne, Body& sphereTwo);

    void calculateForce(Body& body);

    bool onSurface(Body& body);

    void processSurfaceCollision(Body& body);

    /**
     * @brief Detect collision between two spherical bodies.
     * 
     * Uses distance-based sphere overlap testing: two spheres collide if
     * the distance between their centers is less than the sum of their radii.
     * Includes epsilon tolerance to handle floating-point precision issues.
     * 
     * @param sphereOne First body in collision pair
     * @param sphereTwo Second body in collision pair
     * @return true if spheres are overlapping or touching, false otherwise
     */
    bool areColliding(Body& sphereOne, Body& sphereTwo);

    /**
     * @brief Resolve collision between two bodies using impulse-based physics.
     * 
     * Implements elastic collision response by calculating relative velocities
     * along the collision normal, then applying impulses to both bodies based
     * on conservation of momentum and energy. Uses the formula:
     * 
     * v₁' = ((m₁-m₂)/(m₁+m₂)) * v₁ + (2m₂/(m₁+m₂)) * v₂
     * v₂' = (2m₁/(m₁+m₂)) * v₁ + ((m₂-m₁)/(m₁+m₂)) * v₂
     * 
     * @param sphereOne First body in collision (velocity will be modified)
     * @param sphereTwo Second body in collision (velocity will be modified)
     */
    void processCollision(Body& sphereOne, Body& sphereTwo);

    /**
     * @brief Calculate Euclidean distance between centers of two bodies.
     * 
     * Uses squared distance internally to avoid expensive sqrt operation
     * until necessary. Distance is calculated as: d = √((p₁-p₂)⋅(p₁-p₂))
     * 
     * @param sphereOne First body
     * @param sphereTwo Second body
     * @return Distance between body centers in world units
     */
    double getDistance(Body& sphereOne, Body& sphereTwo);
}; 

#endif