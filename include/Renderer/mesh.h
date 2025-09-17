#ifndef MESH_H
#define MESH_H

#include <string>
#include "Sphere3D.h"
#include "Surface3D.h"

// Simple GPU mesh container (one VAO/VBO/EBO + index count)
struct Mesh {
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    int          indexCount = 0;
};

// Sphere instance: owns CPU geometry + its GPU mesh + render properties
struct Sphere {
    Sphere3D     geometry;          // Procedural vertex/index data (CPU side)
    Mesh         mesh;              // Uploaded GPU buffers (lazy created)
    glm::vec3    Color{1.0f};       // Base albedo / emissive tint
    glm::vec3    Position{0.0f};    // World position (no rotation/scale here)
    std::string  Name;              // Debug name
    bool         source = false;    // True = treated as light/emissive
    bool         remake = true;     // True = geometry changed, needs re-upload

    // Default: unit radius sphere
    Sphere() : geometry(1.0f) {}

    Sphere(std::string& name, float radius, glm::vec3 color)
        : geometry(radius), Name(name), Color(color) {}

    Sphere(std::string& name, float radius, glm::vec3 color, glm::vec3 lighting)
        : geometry(radius), Name(name), Color(color), Position(lighting) {}

    // Mark geometry dirty when parameters change
    void setRadius(float radius) {
        geometry.setRadius(radius);
        remake = true;
    }
    void setSubdivisions(unsigned int subs) {
        geometry.setSubdivisions(subs);
        remake = true;
    }
};

struct Surface {
    Surface3D   geometry;  // Procedural Vertex/Index Data
    Mesh        mesh;      // Vertex Buffer data (VAO, VBO, EBO)
    glm::vec3   color;     // Color of the Surface

    Surface() : geometry(-1.0f) { }

    Surface(float distance) : geometry(distance) { }

    Surface(float distance, float size) : geometry(distance, size) { }

    Surface(float distance, float size, surfaceOrientation orientation) : geometry(distance, size, orientation) { }
};

#endif
