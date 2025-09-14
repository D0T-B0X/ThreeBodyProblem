#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"

class App{
public:
    App() {
        renderer.init();
    }

    void run() {

        setupProgram();

        while(!renderer.shouldClose()) {
            renderer.RenderFrame();
        }

        cleanup();
    }

private:
    Renderer renderer;

    Sphere ball_one;
    Sphere ball_two;
    Sphere ball_three;
    Sphere light;

    void setupProgram() {
        ball_one.Name = "Ball One";
        ball_one.source = false;
        ball_one.Color = {0.1f, 0.54f, 1.0f};
        ball_one.setRadius(0.3f);

        ball_two.Name = "Ball Two";
        ball_two.source = false;
        ball_two.Color = {0.1f, 0.54f, 1.0f};
        ball_two.setRadius(0.3f);

        ball_three.Name = "Ball Three";
        ball_three.source = false;
        ball_three.Color = {0.1f, 0.54f, 1.0f};
        ball_three.setRadius(0.3f);

        light.Name = "Light";
        light.source = true;
        light.Color = {1.0f, 1.0f, 1.0f};
        light.setRadius(0.105f);

        renderer.drawSphere(ball_one, {1.0f, 0.0f, 0.0f});
        renderer.drawSphere(ball_two, {-0.5f, 0.0f, 0.8660254f});
        renderer.drawSphere(ball_three, {-0.5f, 0.0f, -0.8660254f});
        renderer.drawSphere(light, {3.0f, 3.0f, 2.0f});
    }

    void cleanup() {
        renderer.cleanup();
    }
};

#endif