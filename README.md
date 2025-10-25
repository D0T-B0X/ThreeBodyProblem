# ThreeBodyProblem

Real-time OpenGL 4.3 simulation for visualizing N-body gravitational dynamics with custom renderer and physics engine. Features procedural geometry, fixed-timestep physics integration, and wireframe grid visualization (~1,600 lines of code).

## Overview
A focused implementation demonstrating:
- Modern OpenGL rendering pipeline (core profile 4.3+)
- Procedural sphere generation via cube subdivision
- **Wireframe grid surfaces** with configurable density
- **Fixed timestep accumulator** for deterministic physics
- FPS camera with mouse look and WASD movement
- Blinn-Phong shading with emissive light sources

## Current Features
### Rendering
- **Procedural geometry**: Spheres (configurable radius/subdivisions) and planar surfaces
- **Wireframe surfaces**: Grid-based mesh with adjustable density (rows × cols)
- **Dynamic lighting**: Single emissive sphere acting as point light source
- **FPS camera**: Full 6DOF movement with mouse look (fixed first-mouse jump issue)
- **Frame timing**: Real-time FPS display in window title

### Physics
- **Fixed timestep integration**: Decoupled physics loop (dt = 1/120s)
- **Position-velocity updates**: Basic kinematic motion (Euler integration)
- **Collision detection**: Sphere-sphere overlap testing via distance comparison
- **Body management**: Vector-based body storage with mass, position, velocity, acceleration, force

### Architecture
- **Modular design**: Separate renderer and physics subsystems
- **Lazy vertex generation**: Deferred sphere mesh creation to avoid double-generation
- **Body-Sphere sync**: Automatic position synchronization between physics and rendering

## In Progress
- [ ] Gravitational force accumulation (Newton's law of gravitation)
- [ ] Collision response with impulse resolution
- [ ] Energy/momentum conservation diagnostics
- [ ] Trajectory trails (line buffer for motion history)

## Roadmap
### Near Term
- Implement pairwise gravitational forces (O(n²))
- Add collision response (elastic/inelastic)
- RK4 integrator option for improved accuracy
- Softening parameter for stability at small distances
- ImGui overlay for runtime parameter tuning

### Long Term
- Spatial partitioning (octree/grid) for O(n log n) broadphase
- Instanced rendering for 1000+ body support
- Multiple light sources (uniform arrays)
- Data export (CSV trajectories, energy graphs)
- Proper surface normals (explicit attribute vs derived)

## Project Structure
```
include/
  application.h          # Main app loop, setup, coordination
  body.h                 # Body struct + BodyConfig
  settings.h             # Global constants (screen size, FOV)
  Renderer/
    renderer.h           # Render loop, sphere/surface drawing
    camera.h             # FPS camera with mouse input
    shader.h             # Shader compilation/uniform helpers
    mesh.h               # Sphere/Surface structs, Mesh container
    Sphere3D.h           # Procedural sphere generation
    Surface3D.h          # Planar surface/wireframe grid
  Physics/
    physics.h            # Physics engine (integration, collision)
src/
  main.cpp               # Entry point
  glad.c                 # OpenGL loader
  Renderer/
    renderer.cpp
    camera.cpp
    shader.cpp
    Sphere3D.cpp
    Surface3D.cpp
  Physics/
    physics.cpp
shaders/
  vObj.glsl              # Vertex shader (MVP transform)
  fObj.glsl              # Fragment shader (Blinn-Phong)
config.h.in → build/config.h  # CMake-generated paths
CMakeLists.txt
LICENSE
```

## Build Instructions

### Dependencies (Debian/Ubuntu)
```bash
sudo apt install build-essential cmake pkg-config libglfw3-dev libglm-dev
```

### Compile
```bash
mkdir -p build
cmake -S . -B build
cmake --build build -j$(nproc)
./build/ThreeBodyProblem
```

## Controls
| Input | Action |
|-------|--------|
| **Mouse** | Camera look (pitch/yaw) |
| **W / S** | Move forward / backward |
| **A / D** | Strafe left / right |
| **Space** | Move up |
| **Left Ctrl** | Move down |
| **ESC** | Quit application |

## Technical Details

### Physics Loop
Fixed timestep accumulator ensures deterministic simulation:
```cpp
accumulator += frameTime;
while (accumulator >= dt) {
    physics.processFrame(bodies);  // Update positions/velocities
    accumulator -= dt;
}
// Sync physics positions to render positions
for (auto& body : bodies) {
    body.sphere.Position = body.Position;
}
renderer.RenderFrame();
```

### Vertex Generation Optimization
Spheres use deferred generation to avoid double-calculation:
1. Default constructor sets radius to `-1.0f` (sentinel value)
2. Vertex generation skipped if radius < 0
3. Generation occurs once when `setRadius()` is called
4. Result: single generation pass with final parameters

### Wireframe Grid
Surface3D supports two modes:
- **Filled**: Standard quad (2 triangles, 4 vertices)
- **Wireframe**: Grid mesh (configurable rows × cols, GL_LINES primitive)

Toggle via `surface.setWireframe(true)` and control density with `setGridDensity(rows, cols)`.

## Known Limitations
- **Single light source** (emissive sphere only)
- **No collision response** (detection only)
- **Derived normals** (spheres use normalized position; surfaces need explicit normals)
- **No gamma correction or HDR**
- **O(n²) collision checks** (needs spatial partitioning for large N)

## Performance Notes
- **~1,600 lines of code** (excluding third-party libraries)
- **60 FPS target** with <16ms frame times
- Tested with up to 4 bodies (3 dynamic + 1 light)
- Wireframe grid: 12×12 tested (144 vertices, 264 line segments)

## Troubleshooting

**Blank screen?**
- Check OpenGL 4.3 support: `glxinfo | grep "OpenGL version"`
- Verify shader compilation (check terminal output)

**Mouse snaps to different angle on startup?**
- Fixed via firstMouse flag (ignores initial callback)

**Bodies don't move?**
- Ensure position sync: `body.sphere.Position = body.Position` before render

**Wireframe not showing?**
- Call `surface.setWireframe(true)` before `drawSurface()`
- Check `surface.mesh.isWireframe` is propagated to renderer

## Future Enhancements
1. **Multi-light support**: Array of light sources with attenuation
2. **Trajectory visualization**: Persistent line rendering for paths
3. **Parameter UI**: ImGui for mass, G, dt, color adjustments
4. **Screenshot/recording**: Frame capture for analysis
5. **Scenario loader**: JSON/config file for initial conditions
6. **Adaptive timestep**: Variable dt with error control

## License
GNU General Public License v3.0 — see [LICENSE](LICENSE)

## Acknowledgements
- **GLFW** — windowing and input handling
- **GLAD** — OpenGL function loader
- **GLM** — mathematics library

## Project Stats
- **Lines of Code**: ~1,600 (excluding libraries)
- **Files**: 10+ source/header files
- **Architecture**: Modular (Renderer + Physics subsystems)
- **Language**: C++17
- **Graphics API**: OpenGL 4.3 Core Profile