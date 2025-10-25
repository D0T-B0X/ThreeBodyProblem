#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"
#include "Physics/physics.h"

class App{
public:
    App() {
        rEngine.init();
        pEngine.init();
    }

    void run() {
 
        setupProgram();

        while(!rEngine.shouldClose()) {
            rEngine.RenderFrame();
            pEngine.processFrame(bodies);
        }

        cleanup();
    }

private:
    Renderer rEngine;
    Physics pEngine;

    std::vector<Body> bodies;

    Sphere ball_one;
    Sphere ball_two;
    Sphere ball_three;
    Sphere light;
    Surface surface;

    void setupProgram() {
        ball_one.Name = "Ball One";             // Set sphere debug name
        ball_one.mesh.source = false;           // Set sphere emission status
        ball_one.Color = {1.0f, 0.0f, 0.0f};    // Set sphere color
        ball_one.setRadius(0.5f);               // Set sphere size

        ball_two.Name = "Ball Two";
        ball_two.mesh.source = false;
        ball_two.Color = {0.0f, 1.0f, 0.0f};
        ball_two.setRadius(0.5f);

        ball_three.Name = "Ball Three";
        ball_three.mesh.source = false;
        ball_three.Color = {0.0f, 0.0f, 1.0f};
        ball_three.setRadius(0.5f);

        // Light source sphere
        light.Name = "Light";
        light.mesh.source = true;
        light.Color = {1.0f, 1.0f, 1.0f};
        light.setRadius(0.5f);

        surface.color = glm::vec3(0.5f, 0.5f, 0.5f); // Surface color
        surface.setSize(40.0f);                      // Surface size (height and width)
        surface.setWireframe(true);                  // Enable wireframe grid
        surface.setGridDensity(12, 12);              // Control grid density
        surface.mesh.inactive = true;                // Surface will not be affected by lighting
        surface.setDistance(-2.0f);                  // Distance of surface from origin (y-axis)

        rEngine.drawSphere(ball_one, {0.0f, 2.0f, -2.0f});
        rEngine.drawSphere(ball_two, {1.7320508075f, -1.0f, -2.0f});
        rEngine.drawSphere(ball_three, {-1.7320508075f, -1.0f, -2.0f});
        rEngine.drawSphere(light, {0.0f, 0.0f, 4.0f});
        rEngine.drawSurface(surface);
    }

    void cleanup() {
        rEngine.cleanup();
        pEngine.cleanup();
    }
};

#endif