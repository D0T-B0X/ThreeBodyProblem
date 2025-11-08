#ifndef MESH_H
#define MESH_H

#include <string>
#include "Sphere3D.h"
#include "Surface3D.h"

// Simple GPU mesh container
struct Mesh {
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    int          indexCount = 0;
    bool         source = false;      // True = treated as light/emissive
    bool         inactive = false;    // True = lighting won't be applied
    bool         remake = true;       // True = geometry changed, needs re-upload
    bool         isWireframe = false; // when true, renderer should draw GL_LINES
};

// Sphere instance: owns CPU geometry + its GPU mesh + render properties
struct Sphere {
    Sphere3D     geometry;          // Procedural vertex/index data (CPU side)
    Mesh         mesh;              // Uploaded GPU buffers (lazy created)
    glm::vec3    Color{1.0f};       // Base albedo / emissive tint
    std::string  Name;              // Debug name

    // Default: unit radius sphere
    Sphere() {}

    Sphere(std::string& name, float radius, glm::vec3 color)
        : geometry(radius), Name(name), Color(color) {}

    Sphere(std::string& name, float radius, glm::vec3 color, glm::vec3 lighting)
        : geometry(radius), Name(name), Color(color) {}

    // Mark geometry dirty when parameters change
    void setRadius(float radius) {
        geometry.setRadius(radius);
        mesh.remake = true;
    }
    void setSubdivisions(unsigned int subs) {
        geometry.setSubdivisions(subs);
        mesh.remake = true;
    }
};

struct Surface {
    Surface3D      geometry;         // Procedural Vertex/Index Data
    Mesh           mesh;             // Vertex Buffer data (VAO, VBO, EBO)
    glm::vec3      color;            // Color of the Surface

    Surface() 
        : geometry(-1.0f) { }

    Surface(float distance) 
        : geometry(distance) { }

    Surface(float distance, float size) 
        : geometry(distance, size) { }

    Surface(float distance, float size, surfaceOrientation orientation)
        : geometry(distance, size, orientation) { }

    void setDistance(float distance) {
        geometry.setDistance(distance);
        mesh.remake = true;
    }

    void setSize(float size) {
        geometry.setSize(size);
        mesh.remake = true;
    }

    void setWireframe(bool wf) {
        geometry.setWireframe(wf);
        mesh.remake = true;
    }

    void setGridDensity(int rows, int cols) {
        geometry.setGridDensity(rows, cols);
        mesh.remake = true;
    }
};

#endif
