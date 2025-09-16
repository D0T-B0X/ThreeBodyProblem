#ifndef SURFACE_3D_H
#define SURFACE_3D_H

#include <vector>
#include <glm/vec3.hpp>

inline constexpr glm::vec3 Xv{0.0f, 1.0f, 1.0f};
inline constexpr glm::vec3 Yv{1.0f, 0.0f, 1.0f};
inline constexpr glm::vec3 Zv{1.0f, 1.0f, 0.0f};

enum surfaceOrientation {
    X,
    Y,
    Z
};

class Surface3D {
public:
    Surface3D();
    Surface3D(float distance);
    Surface3D(float distance, float size);
    Surface3D(float distance, float size, surfaceOrientation orientation);

    const unsigned int* getIndices();
    const float* getVertices();
    const int getVertexSize();
    const int getIndexSize();
    const int getIndexCount();

private:
    float Size = 5.0f;
    float Distance = -2.0f;
    surfaceOrientation Orientation = surfaceOrientation::Y;

    std::vector<unsigned int> Indices;
    std::vector<float> Vertices;

    void generateVertices(surfaceOrientation orientation, float distance, float size);
};

#endif