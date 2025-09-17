#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"
#include "Physics/physics.h"

class App{
public:
    App() {
        renderer.init();
    }

    void run() {

        setupProgram();

        while(!renderer.shouldClose()) {
            renderer.RenderFrame();
            pEngine.processFrame(bodies);
        }

        cleanup();
    }

private:
    Renderer renderer;
    Physics pEngine;

    std::vector<Body> bodies;

    Sphere ball_one;
    Sphere ball_two;
    Sphere ball_three;
    Sphere light;

    void setupProgram() {
        ball_one.Name = "Ball One";
        ball_one.source = false;
        ball_one.Color = {1.0f, 0.0f, 0.0f};
        ball_one.setRadius(0.3f);

        ball_two.Name = "Ball Two";
        ball_two.source = false;
        ball_two.Color = {0.0f, 1.0f, 0.0f};
        ball_two.setRadius(0.3f);

        ball_three.Name = "Ball Three";
        ball_three.source = false;
        ball_three.Color = {0.0f, 0.0f, 1.0f};
        ball_three.setRadius(0.3f);

        light.Name = "Light";
        light.source = true;
        light.Color = {1.0f, 1.0f, 1.0f};
        light.setRadius(0.105f);

        renderer.drawSphere(ball_one, {0.0f, 2.0f, -2.0f});
        renderer.drawSphere(ball_two, {1.7320508075f, -1.0f, -2.0f});
        renderer.drawSphere(ball_three, {-1.7320508075f, -1.0f, -2.0f});
        renderer.drawSphere(light, {0.0f, 0.0f, 4.0f});
    }

    void cleanup() {
        renderer.cleanup();
    }
};

#endif