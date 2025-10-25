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
        ball_one.Name = "Ball One";
        ball_one.mesh.source = false;
        ball_one.Color = {1.0f, 0.0f, 0.0f};
        ball_one.setRadius(0.3f);

        ball_two.Name = "Ball Two";
        ball_two.mesh.source = false;
        ball_two.Color = {0.0f, 1.0f, 0.0f};
        ball_two.setRadius(0.3f);

        ball_three.Name = "Ball Three";
        ball_three.mesh.source = false;
        ball_three.Color = {0.0f, 0.0f, 1.0f};
        ball_three.setRadius(0.3f);

        light.Name = "Light";
        light.mesh.source = true;
        light.Color = {1.0f, 1.0f, 1.0f};
        light.setRadius(0.105f);

        surface.color = glm::vec3(0.5f, 0.5f, 0.7f);
        surface.setSize(40.0f);
        surface.setWireframe(true);          // Enable wireframe grid
        surface.setGridDensity(12, 12);      // Optional: control grid density
        surface.mesh.inactive = true;
        surface.setDistance(-2.0f);

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