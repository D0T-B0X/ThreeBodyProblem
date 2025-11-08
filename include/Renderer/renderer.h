/**
 * @file renderer.h
 * @author DotBox
 * @brief OpenGL rendering subsystem for the three-body gravitational simulator
 * 
 * This module manages all graphics rendering operations including window creation,
 * OpenGL context initialization, shader management, camera control, and frame rendering.
 * It provides a high-level interface for drawing spheres and surfaces with Blinn-Phong
 * lighting, handles user input (keyboard/mouse), and maintains frame timing.
 * 
 * Architecture:
 * - GLFW: Window management and input handling
 * - GLAD: OpenGL function loader (core profile 4.3+)
 * - Camera: FPS-style first-person camera with mouse look
 * - Shader: Wrapper for vertex/fragment shader compilation and uniform management
 * 
 * Rendering pipeline:
 * 1. Process input (keyboard movement, mouse look)
 * 2. Update camera matrices (view/projection)
 * 3. For each registered sphere:
 *    - Set model matrix (position/scale transform)
 *    - Upload uniforms (MVP matrices, colors, lighting)
 *    - Draw sphere geometry (VAO/VBO/EBO)
 * 4. Draw surface (wireframe grid or filled quad)
 * 5. Swap buffers and update frame timing
 * 
 * Lighting model:
 * - Single point light source (emissive sphere)
 * - Blinn-Phong shading: ambient + diffuse + specular components
 * - Sphere normals derived from normalized position vectors
 * 
 * Performance considerations:
 * - Lazy vertex buffer upload (only generates mesh on first draw or geometry change)
 * - Instanced rendering not yet implemented (future optimization for many bodies)
 * - Frame timing calculated each frame for FPS display
 * 
 * @version 0.1
 * @date 2025-10-28
 * 
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>      // OpenGL function loader (must be before GLFW)
#include <GLFW/glfw3.h>     // Window creation and input handling
#include <iostream>
#include <string>
#include <sstream>

#include "shader.h"         // Shader wrapper (compile / link / uniform helpers)
#include "camera.h"         // FPS style camera with mouse look
#include "body.h"           // Body struct containing Sphere + physics state
#include "settings.h"       // Global settings (screen size, FOV, etc.)
#include "config.h"         // CMake-generated configuration (shader paths, etc.)

/**
 * @brief Renderer class - Manages OpenGL rendering, window, camera, and input
 * 
 * Owns the GLFW window, OpenGL context, shader program, and camera instance.
 * Maintains registries of spheres and surfaces to render each frame.
 */
class Renderer {
public:
    /**
     * @brief Construct renderer and initialize OpenGL context
     * 
     * Sets up GLFW window, loads OpenGL functions via GLAD, compiles shaders,
     * configures OpenGL state (depth testing, face culling), and initializes camera.
     * Registers input callbacks for window resize and mouse movement.
     */
    Renderer();

    /**
     * @brief Check if window should close (ESC pressed or X button clicked)
     * 
     * @return true if window close requested, false otherwise
     */
    bool shouldClose() const;

    /**
     * @brief Register a sphere body for rendering
     * 
     * Adds sphere to internal registry and performs lazy vertex buffer upload
     * if mesh hasn't been generated yet. Stores pointer to Body's sphere member,
     * so caller must ensure Body lifetime exceeds Renderer lifetime.
     * 
     * @param body Body containing sphere geometry and physics state
     */
    void drawSphere(Body& body);

    /**
     * @brief Register a surface for rendering
     * 
     * Uploads surface mesh (quad or wireframe grid) to GPU and stores pointer
     * for rendering each frame. Only one surface supported currently.
     * 
     * @param surface Surface instance with geometry and material properties
     */
    void drawSurface(Surface& surface);

    /**
     * @brief Render a single frame with all registered objects
     * 
     * Main rendering function called once per frame:
     * 1. Calculate frame time and update deltaTime
     * 2. Process keyboard input (WASD movement, vertical controls)
     * 3. Clear color and depth buffers
     * 4. Update camera view/projection matrices
     * 5. Render all registered spheres with lighting
     * 6. Render surface (wireframe or filled)
     * 7. Swap front/back buffers
     * 8. Poll GLFW events (input callbacks)
     * 9. Update FPS display in window title
     * 
     * @param bodies Vector of all bodies in simulation (used for position sync)
     */
    void RenderFrame(std::vector<Body*> bodies);

    /**
     * @brief Request window closure programmatically
     * 
     * Sets GLFW window close flag, causing shouldClose() to return true
     * and main loop to exit. Can be called from external logic (e.g., physics).
     */
    void closeRenderer();

    // === Getter Functions ===

    /**
     * @brief Access underlying GLFW window pointer
     * 
     * Useful for advanced input handling or window property queries
     * not exposed by Renderer API.
     * 
     * @return GLFWwindow* Raw pointer to window (nullptr if not initialized)
     */
    GLFWwindow* getWindow();

    /**
     * @brief Get time elapsed between current and previous frame
     * 
     * Used by physics accumulator to maintain fixed timestep simulation.
     * Value updates each RenderFrame() call.
     * 
     * @return Frame time in seconds (e.g., ~0.0166 for 60 FPS)
     */
    double getFrameTime();

    /**
     * @brief Release OpenGL and GLFW resources
     * 
     * Deletes VAOs, VBOs, EBOs for all spheres and surfaces, destroys
     * shader program, terminates GLFW context. Should be called before
     * program exit to prevent resource leaks.
     */
    void cleanup();


private:
    // ===== Core OpenGL State =====

    /** @brief GLFW window handle; nullptr until createGlfwWindow() succeeds */
    GLFWwindow* window = nullptr;
    
    /** @brief FPS camera with WASD movement and mouse-look controls */
    Camera      camera;
    
    /** @brief Compiled shader program (vertex + fragment) for Blinn-Phong lighting */
    Shader      ourShader;

    // ===== Renderable Object Registries =====
    
    /**
     * @brief All spheres to render each frame (references Body::sphere members)
     * 
     * Stores raw pointers to avoid copying heavy mesh data. Pointers remain valid
     * as long as source Body objects aren't destroyed or moved (vector reallocation).
     * Populated by drawSphere() calls, iterated during RenderFrame().
     */
    std::vector<Sphere*> spheres;

    /**
     * @brief Pointer to the emissive sphere (light source) if one exists
     * 
     * Set during drawSphere() when body.sphere.mesh.source == true.
     * Used to extract lightColor uniform for Blinn-Phong calculations.
     * Remains nullptr if no light source is registered.
     */
    Body* lightSphere = nullptr;
    
    /**
     * @brief Pointer to the base surface (ground plane or grid) if one exists
     * 
     * Set during drawSurface() call. Rendered as wireframe grid (GL_LINES)
     * or filled quad (GL_TRIANGLES) depending on Surface::wireframe flag.
     * Remains nullptr if no surface is registered.
     */
    Surface* baseSurface = nullptr;

    // ===== Mouse Input State (for camera look controls) =====
    
    /** @brief Last recorded X mouse position in screen coordinates */
    float lastX = SCR_WIDTH / 2.0f;
    
    /** @brief Last recorded Y mouse position in screen coordinates */
    float lastY = SCR_HEIGHT / 2.0f;
    
    /**
     * @brief Flag to prevent camera jump on first mouse movement
     * 
     * On first mouse input, lastX/lastY are initialized to cursor position
     * instead of computing a delta, preventing the camera from snapping
     * to an incorrect orientation. Reset to false after first movement.
     */
    bool  firstMouse = true;
    
    /** @brief Time elapsed between current and previous frame (in seconds) */
    float deltaTime = 0.0f;
    
    /** @brief Timestamp of last frame (from glfwGetTime()) */
    float lastFrame = 0.0f;

    // ===== Private Helper Methods =====
    
    /**
     * @brief Configure GLFW window hints (OpenGL version, profile, samples)
     * 
     * Sets OpenGL 4.3 Core profile, forward compatibility, and MSAA samples.
     * Called once during Renderer() constructor before window creation.
     */
    void initGlfwWindow();
    
    /**
     * @brief Create GLFW window, bind OpenGL context, and register callbacks
     * 
     * @param width Window width in pixels
     * @param height Window height in pixels
     * @param name Window title string
     * 
     * Enables raw mouse input for smoother camera control. Registers
     * framebufferSizeCallback and mouseCallback for resize/input handling.
     */
    void createGlfwWindow(unsigned int width, unsigned int height,
                          const char* name);
    
    /**
     * @brief Load OpenGL function pointers via GLAD
     * 
     * Must be called after OpenGL context creation (createGlfwWindow).
     * Exits program with error message if loading fails (usually means
     * OpenGL 4.3 not supported by driver).
     */
    void loadGLAD();
    
    /**
     * @brief Update camera matrices and upload to shader uniforms
     * 
     * Calculates view matrix from camera position/orientation and projection
     * matrix from FOV/aspect ratio. Uploads both to "view" and "projection"
     * uniforms in currently bound shader program.
     */
    void generateCameraView();
    
    /**
     * @brief Generate or update sphere's VAO/VBO/EBO on GPU
     * 
     * @param sphere Sphere with mesh data (vertices, normals, indices)
     * 
     * Lazy upload: only generates buffers if mesh.VAO == 0 or geometry changed.
     * Creates vertex array with interleaved position/normal attributes.
     * Used for procedurally generated subdivision spheres.
     */
    void setupSphereVertexBuffer(Sphere& sphere);
    
    /**
     * @brief Generate or update surface's VAO/VBO/EBO on GPU
     * 
     * @param surface Surface with mesh data (vertices, indices, wireframe flag)
     * 
     * Handles both wireframe grids (GL_LINES topology) and filled quads
     * (GL_TRIANGLES). Only uploads if mesh.VAO == 0 or geometry changed.
     */
    void setupSurfaceVertexBuffer(Surface& surface);
    
    /**
     * @brief GLFW callback for window resize events
     * 
     * @param window GLFW window that was resized
     * @param width New framebuffer width in pixels
     * @param height New framebuffer height in pixels
     * 
     * Static callback required by GLFW API. Updates OpenGL viewport to match
     * new window dimensions, ensuring rendering scales correctly.
     */
    static void frameBufferSizeCallback(GLFWwindow* window,
                                        int width, int height);
    
    /**
     * @brief Process keyboard input for camera movement
     * 
     * @param window GLFW window to query key states from
     * 
     * WASD: horizontal movement (forward/back/strafe)
     * Space/Shift: vertical movement (up/down)
     * ESC: request window close
     * 
     * Movement speed scaled by deltaTime for frame-rate independence.
     */
    void processKeyboardInput(GLFWwindow* window);
    
    /**
     * @brief GLFW callback for mouse cursor movement
     * 
     * @param window GLFW window receiving mouse input
     * @param xpos Mouse X position in screen coordinates
     * @param ypos Mouse Y position in screen coordinates
     * 
     * Static callback required by GLFW API. Retrieves Renderer instance
     * from window user pointer and forwards to handleMouse() for processing.
     */
    static void mouseCallback(GLFWwindow* window,
                              double xpos, double ypos);
    
    /**
     * @brief Apply mouse movement delta to camera orientation
     * 
     * @param xpos Current mouse X position
     * @param ypos Current mouse Y position
     * 
     * Calculates offset from last position, applies mouse sensitivity,
     * and updates camera yaw/pitch for free-look control. Handles
     * firstMouse flag to prevent initial camera snap.
     */
    void handleMouse(double xpos, double ypos);
    
    /**
     * @brief Update window title with current FPS
     * 
     * @param deltaTime Time elapsed since last frame
     * 
     * Calculates frames per second and updates GLFW window title string
     * with formatted FPS display. Called each frame during RenderFrame().
     */
    void displayFrameRate(float deltaTime) const;
};

#endif