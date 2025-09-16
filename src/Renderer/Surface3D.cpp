#include "Renderer/Surface3D.h"

Surface3D::Surface3D() {
    generateVertices(Orientation, Distance, Size);
}

Surface3D::Surface3D(float distance) : Distance(distance) {
    generateVertices(Orientation, Distance, Size);
}

Surface3D::Surface3D(float distance, float size) : Distance(distance), Size(size) {
    generateVertices(Orientation, Distance, Size);
}

Surface3D::Surface3D(float distance, float size, surfaceOrientation orientation) : Distance(distance), Size(size), Orientation(orientation) {
    generateVertices(Orientation, Distance, Size);
}

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

void Surface3D::generateVertices(surfaceOrientation orientation, float distance, float size) {

    Vertices.clear();
    Indices.clear();
    Vertices.reserve(4 * 3);
    Indices.reserve(6);

    const float s = size * 0.5f;

    auto push = [&](float x, float y, float z) {
        Vertices.push_back(x);
        Vertices.push_back(y);
        Vertices.push_back(z);
    };

    switch (orientation) {
        case surfaceOrientation::X: {
            float x = distance;
            // Quad on plane X = distance
            push(x, -s, -s); // 0
            push(x,  s, -s); // 1
            push(x,  s,  s); // 2
            push(x, -s,  s); // 3
            break;
        }
        case surfaceOrientation::Y: {
            float y = distance;
            // Quad on plane Y = distance
            push(-s, y, -s); // 0
            push( s, y, -s); // 1
            push( s, y,  s); // 2
            push(-s, y,  s); // 3
            break;
        }
        case surfaceOrientation::Z: {
            float z = distance;
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
}