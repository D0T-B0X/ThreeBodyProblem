# ThreeBodyProblem

OpenGL 4.3 playground for visualizing and (incrementally) simulating a 3 / N‑body gravitational system. Includes a lightweight renderer (GLFW + GLAD) plus a growing physics layer (gravity, collisions, integration).

## Overview
Core goals:
- Modern, minimal rendering path (no legacy fixed pipeline).
- Procedural geometry (subdivided cube → sphere projection, simple quad surface).
- Single light (emissive sphere) + basic Blinn/Phong shading.
- Deterministic, decoupled physics loop (planned) with clear separation from rendering.
- Educational clarity over engine feature breadth.

## Current Features
- Procedural spheres (configurable radius/subdivisions).
- Emissive sphere treated as point light.
- Movable FPS camera (W/A/S/D + Space/Ctrl + mouse look).
- Basic surface (quad) generation (no vertex normals yet).
- Title bar FPS display.
- Physics scaffolding: Body struct + force accumulation placeholders + collision overlap test (sphere–sphere).

## In Progress (Physics)
- Per‑frame force accumulation (gravity not yet applied).
- Sphere–sphere broadphase via radius overlap (areColliding).
- Planned semi‑implicit Euler integration.
- Planned energy / momentum diagnostics.

## Planned Roadmap
Near term:
- [ ] Implement gravitational force accumulation (O(n^2)).
- [ ] Fixed timestep accumulator (e.g., 1/120 s) + render interpolation.
- [ ] Semi‑implicit Euler integration (then optional RK4).
- [ ] Elastic collision response (sphere–sphere).
- [ ] Softening factor to avoid singularities at very small r.
- [ ] Proper normals for surface (currently derived per‑vertex from position only for spheres).
- [ ] Multiple light sources (array uniforms / UBO).
- [ ] Trajectory trail rendering (dynamic line buffer).
- [ ] Parameter controls (G, masses, timestep) via simple UI or config.

Longer term:
- Spatial partition (uniform grid / BVH) for scalable broadphase.
- Instanced rendering for large body counts.
- Optional ImGui overlay (debug + runtime tuning).
- Deterministic replay / state serialization.

## Project Structure
```
include/
  application.h
  settings.h
  body.h
  Renderer/
    renderer.h
    camera.h
    shader.h
    mesh.h
    Sphere3D.h
    Surface3D.h
  Physics/
    physics.h
shaders/
  vObj.glsl
  fObj.glsl
src/
  main.cpp
  glad.c
  Renderer/
    renderer.cpp
    camera.cpp
    shader.cpp
    Sphere3D.cpp
    Surface3D.cpp
  Physics/
    physics.cpp
config.h.in → (CMake) → build/config.h
CMakeLists.txt
LICENSE
```

## Build (Linux)
Dependencies:
```
sudo apt install build-essential cmake pkg-config libglfw3-dev libglm-dev
```
Then:
```
mkdir -p build
cmake -S . -B build
cd build/
make -j $(nproc)
./ThreeBodyProblem
```

## Controls
- Mouse: Look
- W / A / S / D: Move
- Space / Left Ctrl: Up / Down
- ESC: Quit

## Shader Notes
- Normals for spheres: reconstructed by normalizing object‑space position (acceptable because sphere center at transform origin).
- Surface quad: currently also uses position‑derived normal → incorrect lighting; will be replaced with explicit normal attribute.

## Physics Design (Planned Loop)
```
accumulator += realDelta;
while (accumulator >= fixedDt) {
  accumulateForces(bodies);     // gravity, collisions (impulses)
  integrate(bodies, fixedDt);   // semi-implicit Euler
  accumulator -= fixedDt;
}
alpha = accumulator / fixedDt;
render(interpolate(previous, current, alpha));
```
Bodies sync positions to Sphere.Position before rendering.

## Known Limitations
- No numerical integration yet (bodies static).
- Collisions: only overlap test, no response.
- Single point light; emissive sphere also rendered (no shadowing).
- No gamma correction / HDR.
- No normal / tangent attributes (simple shading only).

## Extending
- Add proper vertex normals to Surface3D (supply a second VBO attribute).
- Introduce a Material struct (ambient/diffuse/specular, shininess).
- Introduce multi-light support (struct Light { pos, color }) + uniform array.
- Add command line args (e.g., ./ThreeBodyProblem --bodies 5 --dt 0.0083).

## Troubleshooting
- Blank window: verify GL 4.3 support (glxinfo | grep "OpenGL version").
- Only one triangle for surface: ensure indices = {0,1,2, 0,2,3} and 4 unique vertices.
- Lighting looks inverted: confirm model matrix (no negative scaling) and normal computation.

## License
GNU General Public License v3.0 (see LICENSE).

## Acknowledgements
- GLFW (windowing/input)
- GLAD (OpenGL loader)
- GLM (math)
- References: “Fix Your Timestep” (Gaff / Fiedler) for physics loop pattern.

## Disclaimer
Educational / experimental; APIs unstable while physics layer matures.