#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"
#include "Physics/physics.h"

class App{
public:
    App() : accumulator(0.0f) { }

    void run() {
 
        setupProgram();

        while(!rEngine.shouldClose() && !pEngine.shouldClose()) {
            frameCount++;

            // Time taken between two consecutive frames
            double frameTime = rEngine.getFrameTime();
            accumulator += frameTime;

            // If accumulated time is more than the fixed
            // physics engine time step then run the physics engine
            while (accumulator >= dt) {
                pEngine.processFrame(bodies);
                accumulator -= dt;
            }            
            rEngine.RenderFrame(bodies);
        }

        cleanup();
    }

private:
    Renderer rEngine;
    Physics pEngine;

    std::vector<Body> bodies;

    float accumulator;
    Body ball_one;
    Body ball_two;
    Body ball_three;
    Body light;
    Surface surface;

    void setupProgram() {
        
        // Red ball
        ball_one.sphere.Name = "Ball One";                   // Set sphere debug name
        ball_one.sphere.mesh.source = false;                 // Set sphere emission status
        ball_one.sphere.Color = {1.0f, 0.0f, 0.0f};          // Set sphere color
        ball_one.setRadius(0.5f);                            // Set sphere size
        ball_one.Position = glm::vec3(0.0f, 2.0f, -2.0f);    // Set the ball position
        ball_one.Mass = 1.0f;                                // Set the ball's mass
        ball_one.Velocity = glm::vec3(1.0f, 0.0f, 0.0f);     // Set the ball's initial velocity
        ball_one.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); // Set the ball's initial accelaration
        ball_one.Force = glm::vec3(0.0f, 0.0f, 0.0f);        // Set the initial force acting on the body
        bodies.push_back(ball_one);                          // Add ball to the bodies list

        // Green ball
        ball_two.sphere.Name = "Ball Two";
        ball_two.sphere.mesh.source = false;
        ball_two.sphere.Color = {0.0f, 1.0f, 0.0f};
        ball_two.setRadius(0.5f);
        ball_two.Position = glm::vec3(1.7320508075f, -1.0f, -2.0f);
        ball_two.Mass = 1.0f;                               
        ball_two.Velocity = glm::vec3(-1.0f, 0.0f, 0.0f);     
        ball_two.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
        ball_two.Force = glm::vec3(0.0f, 0.0f, 0.0f);      
        bodies.push_back(ball_two);

        // Blue ball
        ball_three.sphere.Name = "Ball Three";
        ball_three.sphere.mesh.source = false;
        ball_three.sphere.Color = {0.0f, 0.0f, 1.0f};
        ball_three.setRadius(0.5f);
        ball_three.Position = glm::vec3(-1.7320508075f, -1.0f, -2.0f);
        ball_three.Mass = 1.0f;                                
        ball_three.Velocity = glm::vec3(1.0f, 0.0f, 0.0f);     
        ball_three.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
        ball_three.Force = glm::vec3(0.0f, 0.0f, 0.0f);        
        bodies.push_back(ball_three);

        // Light source sphere
        light.sphere.Name = "Light";
        light.sphere.mesh.source = true;
        light.sphere.Color = {1.0f, 1.0f, 1.0f};
        light.setRadius(1.0f);
        light.Position = glm::vec3(0.0f, 0.0f, 4.0f);
        light.Mass = 1.0f;                                
        light.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);    
        light.Acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
        light.Force = glm::vec3(0.0f, 0.0f, 0.0f);       
        bodies.push_back(light);

        // Draw all the generated spheres
        for (Body &body : bodies) {
            rEngine.drawSphere(body);
        }

        surface.color = glm::vec3(0.5f, 0.5f, 0.5f); // Surface color
        surface.setSize(40.0f);                      // Surface size (height and width)
        surface.setWireframe(true);                  // Enable wireframe grid
        surface.setGridDensity(10, 10);              // Control grid density
        surface.mesh.inactive = true;                // Surface will not be affected by lighting
        surface.setDistance(-2.0f);                  // Distance of surface from origin (y-axis)

        rEngine.drawSurface(surface);
    }

    void cleanup() {
        rEngine.cleanup();
        pEngine.cleanup();
    }
};

#endif