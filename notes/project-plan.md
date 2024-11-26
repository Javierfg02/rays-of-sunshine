# Ray of Sunshine - Implementation Plan

## Phase 1: Foundation Setup (First Week)
### 1. OpenGL Setup & Basic Scene
- [x] Qt window and canvas setup (completed)
- [ ] Basic vertex and fragment shaders
```glsl
// Basic vertex shader to test
attribute vec3 position;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
```
- [ ] Test with a single cube primitive
- [ ] Implement camera controls (WASD movement)
- [ ] Add proper error checking using debug.h

### 2. Basic Camera System
- [ ] Set up view and projection matrices
- [ ] Implement camera class with basic movement
- [ ] Test camera controls with simple scene

## Phase 2: Building Generation (Second Week)
### 1. Basic Building Shapes
- [ ] Create building mesh class
- [ ] Generate simple cuboid shapes for buildings
- [ ] Implement building placement along road
- [ ] Add random variations in building heights

### 2. Building Details
- [ ] Add texture coordinates for facades
- [ ] Implement window patterns
- [ ] Add normal mapping for surface detail
- [ ] Create different building styles

## Phase 3: Depth of Field (Third Week)
### 1. FBO Setup
- [ ] Create necessary framebuffers:
  - Main scene FBO
  - Blur FBOs (2 for ping-pong)
- [ ] Add depth texture attachment
- [ ] Implement FBO completeness checks

### 2. DoF Implementation
- [ ] Calculate Circle of Confusion
- [ ] Implement gaussian blur shader
- [ ] Add focal length and aperture controls
- [ ] Blend based on depth values

## Phase 4: Camera Path System (Fourth Week)
### 1. Bezier Curve Implementation
- [ ] Create BezierCurve class
- [ ] Implement curve evaluation
- [ ] Add control points for camera path
- [ ] Implement smooth interpolation

### 2. Camera Integration
- [ ] Connect bezier curve to camera movement
- [ ] Add timing system for path following
- [ ] Implement smooth look-at points
- [ ] Add user controls for path speed

## Required Classes

```cpp
class Building {
    // Handles individual building generation
    void generateMesh();
    void setRandomHeight();
    void addWindows();
};

class CameraSystem {
    // Handles camera movement and bezier path
    void updatePosition(float deltaTime);
    void evaluateBezierPoint(float t);
    void setControlPoints(std::vector<glm::vec3> points);
};

class RenderManager {
    // Handles FBOs and rendering passes
    void initializeFBOs();
    void renderScene();
    void applyDepthOfField();
};
```

## Key Resources to Review
1. Building Generation
- CS 1230 Lab 5 (Mesh generation)
- CS 1230 Project 5 (Shapes)
- [Tutorial on procedural buildings](https://learnopengl.com/Guest-Articles/2022/Procedural-Buildings)

2. Depth of Field
- CS 1230 Lab 11 (Frame Buffers)
- Previous projects' FBO setup
- [LearnOpenGL on framebuffers](https://learnopengl.com/Advanced-OpenGL/Framebuffers)

3. Bezier Curves
- CS 1230 curves lecture notes
- [Bezier curve visualization](https://pomax.github.io/bezierinfo/)

## Testing Plan
1. Building Generation
- Test different building heights
- Verify texture mapping
- Check building placement
- Test performance with many buildings

2. Depth of Field
- Test focus at different distances
- Verify blur quality
- Check performance impact
- Test edge cases (very near/far)

3. Camera Path
- Test smooth movement
- Verify path following
- Check for camera orientation
- Test different curve shapes

## Fallback Plans
If time runs short:
1. Buildings: Simplify to basic cuboids without detailed textures
2. DoF: Use simpler blur algorithm
3. Camera: Use linear interpolation instead of bezier curves

## Implementation Order
1. Start with the camera system and basic cube rendering
2. Add building generation with simple shapes
3. Implement depth of field system
4. Finally, add bezier curve camera movement

## Potential Challenges
1. Building Generation
- Performance with many buildings
- UV mapping for textures
- Memory management for meshes

2. Depth of Field
- FBO setup and management
- Blur quality vs performance
- Proper depth handling

3. Camera Movement
- Smooth interpolation
- Proper timing system
- Handling edge cases

Would you like me to elaborate on any particular phase or provide more specific implementation details for any component?
