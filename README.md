# ThreeBodyProblem

Real-time OpenGL 4.3 simulation visualizing N-body gravitational dynamics with custom renderer and physics engine. Features Newtonian gravity, elastic collisions, surface interactions, and procedural geometry.

## Overview
A comprehensive implementation demonstrating:
- **Modern OpenGL rendering** (core profile 4.3+)
- **Newtonian gravity simulation** with pairwise force calculation
- **Elastic collision response** with position correction
- **Surface collisions** with bounce physics and resting states
- **Procedural sphere generation** via subdivided cube geometry
- **Wireframe grid surfaces** with configurable density
- **Fixed timestep accumulator** for deterministic physics
- **FPS camera** with mouse look and 6DOF movement
- **Blinn-Phong shading** with emissive light sources

## Features

### Physics Engine
- **Gravitational attraction**: Pairwise force calculation between all bodies using scaled gravitational constant
- **Collision detection**: Sphere-sphere and sphere-surface overlap testing
- **Collision response**: 
  - Elastic ball-to-ball collisions with momentum conservation
  - Position-based penetration resolution to prevent jittering
  - Surface bouncing with coefficient of restitution (energy loss)
  - Resting state detection to stop micro-bounces
- **Force accumulation**: Support for gravitational forces, impulses, and external forces
- **Euler integration**: Position and velocity updates with configurable timestep (dt = 1/60s)
- **Distance softening**: Prevents singularities when bodies get too close
- **Boundary detection**: Simulation termination when bodies cross thresholds

### Rendering System
- **Procedural geometry**: 
  - Spheres with configurable radius and subdivision levels
  - Planar surfaces (filled or wireframe grid)
- **Dynamic lighting**: Single emissive sphere as point light source
- **FPS camera**: 
  - Full 6DOF movement (WASD + Space/Ctrl for vertical)
  - Mouse look with pitch/yaw control
  - Smooth movement with deltaTime scaling
- **Real-time metrics**: FPS display updated in window title
- **Lazy vertex generation**: Deferred mesh creation to avoid redundant calculations

### Architecture
- **Modular subsystems**: Clean separation between Renderer and Physics engines
- **Body-Sphere synchronization**: Automatic position sync between physics and rendering
- **Fixed timestep physics**: Deterministic simulation decoupled from variable frame rates
- **Vector-based body management**: Dynamic body storage with support for mass, forces, and velocities

## Current Scene Configuration
**Three-body system in equilateral triangle:**
- **Red ball** (top): Position (0, 15, -2), Mass 30e11 kg
- **Green ball** (bottom-right): Position (8.66, 10, -2), Mass 30e11 kg  
- **Blue ball** (bottom-left): Position (-8.66, 10, -2), Mass 30e11 kg
- **Light source**: Static emissive white sphere
- **Ground surface**: Wireframe grid at y = -2.0

All spheres have radius 0.5 units and start at rest. Gravitational constant scaled to 0.1 for visible interactions at human timescales.

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

### Gravity Implementation
Pairwise gravitational forces using Newton's law:
```cpp
F = G * (m₁ * m₂) / r²
```
- Scaled gravitational constant (G = 0.1) for visible effects at simulation scale
- Distance softening (minimum r² = 1.0) prevents infinite forces at close range
- Forces calculated once per pair using `j = i + 1` loop structure
- Force accumulators reset each frame, accumulated during pair iteration

### Collision Response
**Ball-to-ball collisions:**
- Elastic collision using momentum/energy conservation formulas
- Position-based penetration correction pushes spheres apart
- Prevents jittering by separating overlapping bodies immediately

**Ball-to-surface collisions:**
- Velocity reflection in y-axis only (horizontal motion preserved)
- Coefficient of restitution = 0.8 (20% energy loss per bounce)
- Resting threshold: velocities < 0.1 m/s set to zero
- Position clamping prevents sinking through surface

### Physics Loop
Fixed timestep accumulator pattern:
```cpp
accumulator += frameTime;
while (accumulator >= dt) {
    // Reset forces
    // Calculate pairwise gravity
    // Update velocities and positions (Euler integration)
    // Check surface/sphere collisions
    // Apply collision responses
    accumulator -= dt;
}
renderer.RenderFrame();
```

Key parameters:
- **dt**: 1/60 seconds (60 Hz physics)
- **Speed multiplier**: 3.0x (affects position updates only)
- **Velocity decay**: Disabled (λ = 0.0)

### Vertex Generation Optimization
Spheres use deferred generation:
1. Default radius = -1.0 (sentinel value)
2. Vertex generation skipped if radius < 0
3. `setRadius()` triggers one-time mesh generation
4. Result cached in VAO/VBO for subsequent frames

### Wireframe Grid System
Surface3D dual-mode rendering:
- **Filled mode**: Single quad (2 triangles, 4 vertices)
- **Wireframe mode**: Grid mesh (configurable rows × cols, GL_LINES primitive)
- Grid density controlled via `setGridDensity(rows, cols)`
- Orientation support: XY, XZ, YZ planes

## Known Limitations & Solutions

### Current Constraints
- **Single light source** (one emissive sphere)
- **Euler integration** (first-order accuracy, potential energy drift)
- **O(n²) collision/gravity** (all pairs checked every frame)
- **Derived normals** (spheres use normalized position; surfaces lack explicit normals)
- **No spatial partitioning** (broadphase optimization needed for 100+ bodies)
- **No trajectory visualization** (motion history not recorded)

### Debugging Solutions Applied
**Balls stopped moving unexpectedly:**
- Fixed `isZero()` modifying vectors → now read-only check
- Moved boundary check outside inner loops
- Fixed `i` vs `j` typo in gravity loop

**Balls sank through surface:**
- Added position clamping: `body.Position.y = surfaceY + radius`
- Implemented resting threshold to stop micro-bounces

**Runaway velocities/explosion:**
- Added distance softening: `r² = max(r², 1.0)` prevents division by near-zero
- Reduced scaled G from 10.0 → 0.1 to compensate for speed multiplier
- Limited gravity calculation to non-light-source bodies

**Balls merged and jittered:**
- Position correction in collision response pushes spheres apart
- Prevents stuck-together oscillation

## Performance Notes
- **~2,000 lines of code** (excluding third-party libraries)
- **60 FPS target** sustained with 4 bodies (3 dynamic + 1 light)
- Physics: 60 Hz fixed timestep, <0.1ms per frame
- Rendering: ~15ms per frame (includes vsync wait)
- Tested configurations:
  - 3 spheres (radius 0.5, 6 subdivision levels) + wireframe grid
  - Grid density: 20×20 (400 vertices, 760 line segments)

## Roadmap

### Completed ✓
- [x] Gravitational force accumulation (Newton's law)
- [x] Elastic collision response with momentum conservation
- [x] Surface collision with bounce physics
- [x] Position-based penetration correction
- [x] Distance softening for numerical stability
- [x] Resting state detection

### Near Term
- [ ] RK4/Verlet integrator for improved accuracy
- [ ] ImGui overlay for runtime parameter tuning (mass, G, dt, colors)
- [ ] Trajectory trails (line buffer for motion history)
- [ ] Energy/momentum conservation diagnostics
- [ ] Multiple collision iterations per frame

### Long Term  
- [ ] Spatial partitioning (octree/BVH) for O(n log n) broadphase
- [ ] Instanced rendering for 1000+ body support
- [ ] Multiple light sources with attenuation
- [ ] Data export (CSV trajectories, energy graphs, JSON scenarios)
- [ ] Adaptive timestep with error control
- [ ] Explicit surface normals for proper lighting

## Troubleshooting

**Blank screen?**
- Check OpenGL 4.3 support: `glxinfo | grep "OpenGL version"`
- Verify shader compilation (check terminal output)

**Mouse snaps to different angle on startup?**
- Fixed via firstMouse flag (ignores initial callback)

**Bodies don't move?**
- Check if `isZero()` is modifying velocities (should be read-only)
- Ensure gravity loop uses `j = i + 1` (not `j = 0`)

**Balls sink through surface?**
- Position clamping should be: `body.Position.y = surfaceY + radius` (not `<` comparison)
- Surface collision must reverse y-velocity: `body.Velocity.y *= -0.8f`

**Balls explode/fly off screen?**
- Check distance softening: `fDistanceSq = max(fDistanceSq, 1.0f)`
- Verify scaled G is reasonable (0.01 to 1.0 for typical masses)
- Ensure light source is skipped in gravity calculation

**Wireframe not showing?**
- Call `surface.setWireframe(true)` before `drawSurface()`
- Check `surface.mesh.isWireframe` flag propagates to renderer

## Future Enhancements
1. **Verlet/RK4 integration**: Better energy conservation and numerical stability
2. **Multi-light support**: Array of light sources with distance attenuation
3. **Parameter UI**: ImGui panels for mass, G, dt, color adjustments
4. **Trajectory visualization**: Circular buffer + line strip rendering for paths
5. **Screenshot/recording**: Frame capture for analysis/presentation
6. **Scenario loader**: JSON/config files for initial conditions
7. **Collision islands**: Skip processing for resting/static groups

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