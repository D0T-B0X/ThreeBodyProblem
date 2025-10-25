#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>      // OpenGL function loader
#include <GLFW/glfw3.h>     // Window / input
#include <iostream>
#include <string>
#include <sstream>

#include "shader.h"         // Shader wrapper (compile / link / uniform helpers)
#include "camera.h"         // FPS style camera
#include "body.h"
#include "settings.h"       // Global settings (screen size, FOV, etc.)
#include "config.h"         // CMake‑generated (paths, if any)

// Renderer: owns window, GL context, shader, camera, and sphere registry
class Renderer {
public:
    Renderer();

    // check if the window should close (program end)
    bool shouldClose() const;

    // Register a sphere instance at a position (uploads mesh if needed)
    void drawSphere(Body& body);

    void drawSurface(Surface& surface);

    // Render a single frame (poll events, render, swap buffers)
    void RenderFrame(std::vector<Body>& bodies);

    // public API to close the renderer without keyboard input.
    void closeRenderer();

    // Getter function

    // Access underlying GLFW window (e.g., for additional user logic)
    GLFWwindow* getWindow();

    // Access the time between two consecutive frames
    double getFrameTime();

    // Release GL + GLFW resources
    void cleanup();


private:
    // --- Core state ---
    GLFWwindow* window = nullptr;
    Camera      camera;
    Shader      ourShader;

    // All spheres submitted for rendering (stored as pointers; lifetime managed by caller)
    std::vector<Sphere*> spheres;

    // Pointer to the sphere acting as light source
    Sphere* lightSphere = nullptr;
    Surface* baseSurface = nullptr;

    // Mouse state
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool  firstMouse = true;

    // Frame timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // --- Internal helpers ---
    void initGlfwWindow();                                        // Setup GLFW hints
    void createGlfwWindow(unsigned int width, unsigned int height,
                          const char* name);                      // Create + bind context + callbacks
    void loadGLAD();                                              // Load GL function pointers
    void generateCameraView();                                    // Upload view/projection matrices
    void setupSphereVertexBuffer(Sphere& sphere);                 // Lazy (re)upload sphere mesh
    void setupSurfaceVertexBuffer(Surface& surface);
    static void frameBufferSizeCallback(GLFWwindow* window,
                                        int width, int height);   // Resize viewport
    void processKeyboardInput(GLFWwindow* window);                // WASD / vertical movement
    static void mouseCallback(GLFWwindow* window,
                              double xpos, double ypos);          // Static → instance redirect
    void handleMouse(double xpos, double ypos);                   // Apply mouse delta to camera
    void displayFrameRate(float deltaTime) const;                 // Title bar FPS update
};

#endif