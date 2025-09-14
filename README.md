# ThreeBodyProblem

Minimal OpenGL application to visualize and (incrementally) simulate an N / 3‑body gravitational system with a custom lightweight renderer and forthcoming basic physics engine (gravity, integration, collisions).

## Overview
The project provides:
- A focused OpenGL 4.3 core profile renderer (GLFW + GLAD + modern shader pipeline).
- Procedural sphere geometry (cube → sphere subdivision).
- Camera with FPS navigation.
- Simple light sphere (emissive) + shaded bodies.
- Scaffold for adding physically based motion and interactions.

## Current Features
- Real‑time rendering of multiple spheres.
- Distinct light source sphere (emissive branch in shader).
- Procedural mesh generation with lazy GPU upload.
- Camera controls (mouse look, WASD + vertical movement).
- Frame time / FPS display in window title.
- Clean separation of rendering, geometry, and application setup.

## Upcoming Physics Engine (Planned)
Planned iterative additions:
1. Gravitational acceleration (Newtonian, pairwise).
2. Numerical integration (Euler → Semi-Implicit → RK4 option).
3. Basic elastic sphere–sphere collision response.
4. Energy / momentum diagnostics.
5. Adjustable gravitational constant, mass, timestep.
6. Optional softening term to stabilize near-singular interactions.
7. Pause / step / reset controls.
8. Deterministic reproducibility (fixed timestep accumulator).

## Project Structure
```
include/
  Renderer/
    renderer.h        (render loop + resource management)
    camera.h
    shader.h
    cubesphere.h
  application.h       (App bootstrap)
src/
  Renderer/
    renderer.cpp
    camera.cpp
    shader.cpp
    cubesphere.cpp
  glad.c
  main.cpp
shaders/
  vObj.glsl
  fObj.glsl
config.h.in → (CMake) → build/config.h
CMakeLists.txt
```

## Build (Linux)
Dependencies: CMake ≥ 3.19, a C++17 compiler, pkg-config, glfw3, GL, dl.

Commands:
```
mkdir -p build
cd build
cmake ..
cmake --build .
./ThreeBodyProblem
```

## Controls
- Mouse: Look
- W / A / S / D: Horizontal movement
- Space / Left Ctrl: Up / Down
- ESC: Quit

## Configuration
Shader paths are injected via CMake into config.h:
```
#define VSHADER_PATH ".../shaders/vObj.glsl"
#define FSHADER_PATH ".../shaders/fObj.glsl"
```
Adjust sphere properties (color, radius, initial placement) in application.h.

## Physics Extension Plan
New module (planned): physics/
- Body struct: mass, position, velocity, force accumulator.
- Integrator: step(dt).
- Broad phase (simple O(n^2) first; later optional spatial partition).
Renderer will read updated positions each frame.

## Extending
- Add a UI layer (e.g., ImGui) for runtime tweaking.
- Implement trajectory trails (persisted line vertex buffer).
- Add logging of conserved quantities (CSV export).

## Roadmap (Short)
- [ ] Mass assignment per sphere
- [ ] Gravity accumulator
- [ ] Stable fixed timestep loop
- [ ] Collision detection / response
- [ ] RK4 integrator option
- [ ] Command-line overrides (timestep, G, bodies)
- [ ] Trajectory visualization

## Quality / Performance Notes
- Current integrator: none (static placement). First addition will include fixed timestep logic decoupled from render delta.
- Sphere geometry is static.
- Collision handling initially naive (pairwise sphere intersection).

## License
GNU General Public License v3.0

## Acknowledgements
- GLFW (window + input)
- GLAD (OpenGL loader)
- GLM (math library)