#include "Renderer/Surface3D.h"
#include <algorithm>

Surface3D::Surface3D() {
    generateVertices();
}

Surface3D::Surface3D(float distance) : Distance(distance) {
    generateVertices();
}

Surface3D::Surface3D(float distance, float size) : Distance(distance), Size(size) {
    generateVertices();
}

Surface3D::Surface3D(float distance, float size, surfaceOrientation orientation) : Distance(distance), Size(size), Orientation(orientation) {
    generateVertices();
}

// Getter functions
const unsigned int* Surface3D::getIndices() {
    return Indices.data();
}

const float* Surface3D::getVertices() {
    return Vertices.data();
} 

const int Surface3D::getVertexSize() {
    return Vertices.size() * sizeof(float);
}

const int Surface3D::getIndexSize() {
    return Indices.size() * sizeof(unsigned int);
}

const int Surface3D::getIndexCount() {
    return Indices.size();
}

// Setter functions
void Surface3D::setWireframe(bool wf) {
    Wireframe = wf;
    generateVertices();
} 

void Surface3D::setGridDensity(int rows, int cols) {
    GridRows = rows; 
    GridCols = cols; 
    generateVertices();
}

void Surface3D::setDistance(float distance) {
    Distance = distance;
    generateVertices();
}

void Surface3D::setSize(float size) {
    Size = size;
    generateVertices();
}

bool Surface3D::isWireframe() const {
    return Wireframe;
}

void Surface3D::generateVertices() {

    Vertices.clear();
    Indices.clear();

    if (!Wireframe) {
        // Single quad composed of 2 triangles
        Vertices.reserve(4 * 3);
        Indices.reserve(6);

        const float s = Size * 0.5f;

        auto push = [&](float x, float y, float z) {
            Vertices.push_back(x);
            Vertices.push_back(y);
            Vertices.push_back(z);
        };

        switch (Orientation) {
            case surfaceOrientation::X: {
                float x = Distance;
                // Quad on plane X = distance
                push(x, -s, -s); // 0
                push(x,  s, -s); // 1
                push(x,  s,  s); // 2
                push(x, -s,  s); // 3
                break;
            }
            case surfaceOrientation::Y: {
                float y = Distance;
                // Quad on plane Y = distance
                push(-s, y, -s); // 0
                push( s, y, -s); // 1
                push( s, y,  s); // 2
                push(-s, y,  s); // 3
                break;
            }
            case surfaceOrientation::Z: {
                float z = Distance;
                // Quad on plane Z = distance
                push(-s, -s, z); // 0
                push( s, -s, z); // 1
                push( s,  s, z); // 2
                push(-s,  s, z); // 3
                break;
            }
        }

        // Two CCW triangles (0,1,2) and (0,2,3)
        Indices = {0,1,2, 0,2,3};
    } else {
        // Wireframe grid generation (line segments). Produce a regular grid of vertices
        // with GridRows x GridCols cells and emit horizontal + vertical line segments.
        const int rows = std::max(1, GridRows);
        const int cols = std::max(1, GridCols);
        const int vertRows = rows + 1;
        const int vertCols = cols + 1;

        Vertices.reserve(vertRows * vertCols * 3);

        const float half = Size * 0.5f;
        const float dx = Size / cols;
        const float dz = Size / rows;

        // Build vertex grid
        for (int r = 0; r < vertRows; ++r) {
            for (int c = 0; c < vertCols; ++c) {
                float x = -half + c * dx;
                float z = -half + r * dz;
                if (Orientation == surfaceOrientation::Y) {
                    Vertices.push_back(x);
                    Vertices.push_back(Distance);
                    Vertices.push_back(z);
                } else if (Orientation == surfaceOrientation::X) {
                    Vertices.push_back(Distance);
                    Vertices.push_back(x);
                    Vertices.push_back(z);
                } else { // Z
                    Vertices.push_back(x);
                    Vertices.push_back(z);
                    Vertices.push_back(Distance);
                }
            }
        }

        Indices.reserve((vertRows * (vertCols - 1) + vertCols * (vertRows - 1)) * 2);

        // horizontal lines
        for (int r = 0; r < vertRows; ++r) {
            for (int c = 0; c < vertCols - 1; ++c) {
                unsigned int a = r * vertCols + c;
                unsigned int b = a + 1;
                Indices.push_back(a);
                Indices.push_back(b);
            }
        }

        // vertical lines
        for (int c = 0; c < vertCols; ++c) {
            for (int r = 0; r < vertRows - 1; ++r) {
                unsigned int a = r * vertCols + c;
                unsigned int b = (r + 1) * vertCols + c;
                Indices.push_back(a);
                Indices.push_back(b);
            }
        }
    }
}