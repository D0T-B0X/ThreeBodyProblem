/**
 * @file application.h
 * @author DotBox
 * @brief Main application orchestrator for the three-body gravitational simulator
 * 
 * This is the core control module that coordinates the rendering and physics subsystems.
 * It implements the main game loop using a fixed timestep accumulator pattern to ensure
 * deterministic physics simulation decoupled from variable frame rates.
 * 
 * Architecture:
 * - Renderer (rEngine): Handles all OpenGL rendering, camera, and visual output
 * - Physics (pEngine): Manages numerical integration, forces, and collision detection
 * - Bodies: Vector of physical objects that exist in both render and physics contexts
 * 
 * The loop structure follows the "Fix Your Timestep" pattern:
 * 1. Accumulate real frame time
 * 2. Process physics in fixed dt chunks while accumulator >= dt
 * 3. Sync physics state to render state
 * 4. Render single frame with current state
 * 
 * This ensures physics calculations happen at a constant rate (e.g., 60 Hz) regardless
 * of rendering performance, maintaining consistent behavior across different hardware.
 * 
 * Initial scene setup:
 * - Three colored spheres (red, green, blue) arranged in equilateral triangle
 * - One emissive white sphere acting as point light source
 * - Wireframe grid surface for spatial reference
 * 
 * @version 0.1
 * @date 2025-10-28
 * 
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"
#include "Physics/physics.h"

class App{
public:
    App() : accumulator(0.0f), timeCount(0) { }

    /**
     * @brief Main application loop - orchestrates rendering and physics
     * 
     * Implements fixed timestep game loop pattern:
     * 1. Setup: Initialize scene objects (spheres, surface, lighting)
     * 2. Loop: While neither renderer nor physics requests termination:
     *    a. Measure frame time (variable based on render performance)
     *    b. Accumulate time into physics accumulator
     *    c. Apply impulses or forces at specific frame counts (testing/demo)
     *    d. Process physics in fixed dt increments (deterministic updates)
     *    e. Render current frame state (interpolation could be added here)
     * 3. Cleanup: Release resources for both subsystems
     * 
     * The fixed timestep ensures physics behaves identically regardless of
     * frame rate variations. For example, at 60 FPS physics and 120 FPS render,
     * physics runs once per 2 render frames. At 30 FPS render, physics runs
     * twice per render frame to maintain temporal accuracy.
     */
    void run() {

        setupProgram();

        float multiplier = 2.0f;

        while(!rEngine.shouldClose() && !pEngine.shouldClose()) {
            if (timeCount > 120) {
                // Time taken between two consecutive frames
                double frameTime = rEngine.getFrameTime();
                accumulator += frameTime;

                // Demo: Apply impulse to red ball after 2 seconds (at 60Hz physics)
                if (timeCount == 121) {
                    pEngine.push(ball_one, glm::vec3(multiplier * 1.0f, multiplier * -0.7071f, 0.0f));
                    pEngine.push(ball_two, glm::vec3(multiplier * -0.7071f, multiplier * -0.7071f, 0.0f));
                    pEngine.push(ball_three, glm::vec3(multiplier * 0.7071f, multiplier * 0.7071f, 0.0f));
                }

                // Fixed timestep physics loop: process physics at constant rate
                // regardless of rendering frame rate (ensures determinism)
                while (accumulator >= dt) {

                    pEngine.processFrame(bodies);
                    accumulator -= dt;
                }            
            }
            timeCount++;
            rEngine.RenderFrame(bodies);
        }

        cleanup();
    }

private:
    // Core subsystems
    Renderer rEngine;            ///< OpenGL rendering engine (camera, shaders, draw calls)
    Physics pEngine;             ///< Physics engine (integration, forces, collisions)

    // Scene objects
    std::vector<Body*> bodies;    ///< All physical bodies in the simulation (rendered + physics)
 
    // Timing and state
    float accumulator;           ///< Accumulated real time for fixed timestep processing
    unsigned int timeCount;      ///< Number of physics timesteps executed (frame counter)
    
    // Individual body instances (initialized in setupProgram)
    // The user may create more or less depending on their need.
    Body ball_one;               ///< Red sphere (primary test subject for impulses)
    Body ball_two;               ///< Green sphere (positioned at equilateral triangle vertex)
    Body ball_three;             ///< Blue sphere (positioned at equilateral triangle vertex)
    Body light;                  ///< White emissive sphere (acts as point light source)
    Surface surface;             ///< Ground plane with wireframe grid visualization
    Surface wallOne = Surface(2.0f, 50.0f, surfaceOrientation::X);
    Surface wallTwo;
    Surface wallThree;

    void setupProgram() {
        
        // === Red Ball Configuration ===
        ball_one.sphere.Name = "Red ball";                   // Debug identifier for logging/errors
        ball_one.sphere.mesh.source = false;                 // Not a light source (receives lighting)
        ball_one.sphere.Color = {1.0f, 0.0f, 0.0f};          // Pure red diffuse color
        ball_one.setRadius(0.5f);                            // 0.5 unit radius sphere
        ball_one.Position = glm::vec3(0.0f, 2 * 18.0f, -2.0f);   // Top vertex of equilateral triangle
        ball_one.Mass = 30e11f;                              // 30e11 kg mass
        ball_one.Velocity = glm::vec3(0);                    // Initially at rest
        ball_one.Acceleration = glm::vec3(0);                // No forces applied yet
        ball_one.Force = glm::vec3(0);                       // Force starts at zero
        ball_one.vForceAccumulator = glm::vec3(0);           // Stores all non natural forces
        bodies.push_back(&ball_one);                         // Register with simulation

        // === Green Ball Configuration ===
        ball_two.sphere.Name = "Green ball";
        ball_two.sphere.mesh.source = false;
        ball_two.sphere.Color = {0.0f, 1.0f, 0.0f};          // Pure green diffuse color
        ball_two.setRadius(0.5f);
        ball_two.Position = glm::vec3(2 * 8.66f, 2 * 10.0f, -2.0f);  // Right vertex (10 * √3/2 ≈ 8.66)
        ball_two.Mass = 30e11f;                               
        ball_two.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);     
        ball_two.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
        ball_two.Force = glm::vec3(0.0f, 0.0f, 0.0f);      
        bodies.push_back(&ball_two);

        // === Blue Ball Configuration ===
        ball_three.sphere.Name = "Blue ball";
        ball_three.sphere.mesh.source = false;
        ball_three.sphere.Color = {0.0f, 0.0f, 1.0f};        // Pure blue diffuse color
        ball_three.setRadius(0.5f);
        ball_three.Position = glm::vec3(2 * -8.66f, 2 * 10.0f, -2.0f); // Left vertex (-10 * √3/2 ≈ -8.66)
        ball_three.Mass = 30e11f;                                
        ball_three.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);     
        ball_three.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
        ball_three.Force = glm::vec3(0.0f, 0.0f, 0.0f);        
        bodies.push_back(&ball_three);

        // === Light Source Configuration ===
        light.sphere.Name = "Light";
        light.sphere.mesh.source = true;                     // Emissive: doesn't receive lighting, emits light
        light.sphere.Color = {1.0f, 1.0f, 1.0f};             // White light (neutral color temperature)
        light.setRadius(1.0f);                               // Larger radius for visibility
        light.Position = glm::vec3(0.0f, 0.0f, 4.0f);        // Behind camera/above scene
        light.Mass = 1.0f;                                
        light.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);        // Stationary light source
        light.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        light.Force = glm::vec3(0.0f, 0.0f, 0.0f);       
        bodies.push_back(&light);

        // Register all spheres with renderer for drawing
        for (Body* body : bodies) {
            rEngine.drawSphere(*body);
        }

        // === Ground Surface Configuration ===
        surface.color = glm::vec3(0.5f, 0.5f, 0.5f);     // Medium gray for neutral reference
        surface.setSize(100.0f);                          // 40×40 unit plane (width × height)
        surface.setWireframe(true);                      // Render as grid lines (not filled quads)
        surface.setGridDensity(20, 20);                  // 10×10 grid (11 lines each direction)
        surface.mesh.inactive = true;                    // Unlit surface (no Blinn-Phong shading)
        surface.setDistance(-2.0f);                      // Plane at y = -2 (below origin)

        wallOne.color = glm::vec3(0.0f, 0.5f, 0.5f);
        wallOne.setSize(50.0f);
        wallOne.setWireframe(false);
        wallOne.mesh.inactive = true;

        rEngine.drawSurface(wallOne);
        rEngine.drawSurface(surface);
    }

    void cleanup() {
        rEngine.cleanup();
        pEngine.cleanup();
    }
};

#endif