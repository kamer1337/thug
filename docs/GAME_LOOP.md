# THUG Game Loop Architecture

## Overview

This document describes the frame processing and update cycle in Tony Hawk's Underground (THUG). The game loop is the heart of the engine, coordinating all subsystems to produce smooth, interactive gameplay at target framerates.

## Table of Contents

- [Main Loop Structure](#main-loop-structure)
- [Frame Execution Flow](#frame-execution-flow)
- [Task System](#task-system)
- [Timing and Synchronization](#timing-and-synchronization)
- [Profiling Support](#profiling-support)
- [Platform-Specific Considerations](#platform-specific-considerations)
- [Performance Optimization](#performance-optimization)

---

## Main Loop Structure

### Core Loop

The main game loop is implemented in `Code/Gel/MainLoop/Mainloop.cpp` in the `Manager::MainLoop()` function:

```cpp
void Manager::MainLoop(void)
{
    done = false;
    
    while (!done)
    {
        // 1. Pre-render (platform-specific)
        Nx::CEngine::sPreRender();
        
        // 2. Service system tasks (input, network, etc.)
        service_system();
        
        // 3. Execute game logic
        game_logic();
        
        // 4. Render the world
        Nx::CEngine::sRenderWorld();
        
        // 5. Post-render processing
        Nx::CEngine::sPostRender();
        
        // 6. Frame timing
        Tmr::OncePerRender();
    }
}
```

### Loop Phases

Each frame goes through distinct phases:

1. **Pre-Render**: Set up for rendering previous frame's data
2. **System Services**: Handle input, network, system events
3. **Game Logic**: Update game state, physics, AI
4. **Render World**: Submit rendering commands
5. **Post-Render**: Finalize rendering, display frame
6. **Timing**: Synchronize with target framerate

---

## Frame Execution Flow

### Detailed Frame Pipeline

```
┌─────────────────────────────────────────────────────┐
│                   Frame N Start                     │
└──────────────────┬──────────────────────────────────┘
                   ↓
         ┌─────────────────────┐
         │   Pre-Render Phase  │
         │  (Platform-Specific) │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │  Wait for Previous  │
         │   Frame's DMA/GPU   │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │   VSync/Frame Lock  │
         │   (if enabled)      │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │  Service System     │
         │  - Input            │
         │  - Network          │
         │  - System Events    │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │   Game Logic        │
         │  - Physics          │
         │  - AI               │
         │  - Scripts          │
         │  - Game State       │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │   Render World      │
         │  - Culling          │
         │  - Scene Graph      │
         │  - Submit Draw      │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │   Post-Render       │
         │  - Profiler Display │
         │  - Debug Info       │
         └──────────┬───────────┘
                   ↓
         ┌─────────────────────┐
         │   Frame Timing      │
         │  - Throttling       │
         │  - Metrics          │
         └──────────┬───────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│                   Frame N End                       │
│              (Frame N+1 Begins)                     │
└─────────────────────────────────────────────────────┘
```

---

## Task System

### Task Stacks

The game loop uses **three task stacks** to organize work:

1. **System Task Stack** (`system_task_stack`)
   - Input processing
   - Network updates
   - System-level services
   - Runs first each frame

2. **Logic Task Stack** (`logic_task_stack`)
   - Game state updates
   - Physics simulation
   - AI processing
   - Script execution
   - Collision detection

3. **Display Task Stack** (`display_task_stack`)
   - Scene rendering
   - UI rendering
   - Debug visualization
   - Can be paused independently

### Task Processing

```cpp
inline void Manager::service_system(void)
{
    system_task_stack.Process(currently_profiling);
}

inline void Manager::game_logic(void)
{
    logic_task_stack.Process(currently_profiling);
}

inline void Manager::render_frame(void)
{
    if (!display_tasks_paused)
    {
        if (start_render_hook)
            start_render_hook->Call();
        
        display_task_stack.Process(currently_profiling);
        
        if (end_render_hook)
            end_render_hook->Call();
    }
}
```

### Task Execution Order

**Critical**: Tasks execute in priority order within each stack:
- Higher priority tasks run first
- Tasks can add/remove other tasks
- Task state persists between frames

### Render Hooks

The rendering phase supports customization via hooks:

- **`start_render_hook`**: Called before rendering begins
  - Clear buffers
  - Set up render state
  - Initialize frame

- **`end_render_hook`**: Called after rendering completes
  - Present frame
  - Swap buffers
  - Post-processing

---

## Timing and Synchronization

### Frame Rate Control

The engine supports **framerate locking** via scripting:

```cpp
int framerate = Script::GetInteger("lock_framerate");
if (framerate)  // If locked
{
    static uint64 next_vblanks = 0;
    Tmr::VSync();
    while (Tmr::GetVblanks() < next_vblanks)
    {
        // Wait for next VBlank
    }
    next_vblanks = Tmr::GetVblanks() + framerate;
}
else  // Free-running
{
    Tmr::VSync1();
}
```

### Target Framerates

**Console targets** (original platforms):
- **60 FPS**: Preferred target
- **30 FPS**: Fallback for heavy scenes
- Variable framerate based on scene complexity

**PC target**:
- Flexible framerate
- VSync optional
- Can exceed console limitations

### Frame Timing Functions

- **`Tmr::VSync()`**: Wait for vertical blank
- **`Tmr::VSync1()`**: Single VSync call
- **`Tmr::GetVblanks()`**: Get VBlank counter
- **`Tmr::OncePerRender()`**: Per-frame timing bookkeeping

---

## Profiling Support

### Built-In Profiler

The engine includes a **hierarchical profiler** for performance analysis:

```cpp
#ifdef __USE_PROFILER__
    Sys::CPUProfiler->PushContext(255, 0, 0);  // Red context
    // ... code to profile ...
    Sys::CPUProfiler->PopContext();
#endif
```

### Profiling Contexts

Color-coded profiling contexts:

| Color              | RGB          | Purpose                |
|--------------------|--------------|------------------------|
| Red                | (255, 0, 0)  | System tasks           |
| Green              | (0, 255, 0)  | Game logic             |
| Blue               | (0, 0, 255)  | Wait for GPU/DMA       |
| Yellow             | (255, 255, 0)| Render world           |
| Black (under Y)    | (0, 0, 0)    | Post-render            |

### Profiling API

```cpp
// Start profiling for N frames
Manager::ProfileTasks(n);

// Check if currently profiling
if (currently_profiling) {
    // Detailed logging
}

// Frame start/end markers
Sys::Profiler::sStartFrame();
```

### Profiler Display

The profiler renders on-screen:
- Bar graph showing frame breakdown
- Color-coded by subsystem
- Shows bottlenecks visually
- Rendered during post-render phase

---

## Platform-Specific Considerations

### Win32 Platform

**Current State**: Many Win32 functions are stubbed

```cpp
#ifndef __PLAT_WN32__
    // Console-specific code
    Sys::CPUProfiler->PushContext(...);
#endif

#ifdef __PLAT_WN32__
    // Win32-specific code
#endif
```

**Key Differences**:
- No built-in profiler support (yet)
- Different VSync handling
- Vulcan rendering pipeline
- Windows message pump integration

See [platforms/WIN32.md](platforms/WIN32.md) for details.

### Frame Pacing

**Win32 Implementation Needs**:
- High-resolution timer (`QueryPerformanceCounter`)
- VSync control via DirectX
- DWM (Desktop Window Manager) interaction
- Frame throttling mechanism

---

## Performance Optimization

### Critical Optimization Points

#### 1. Task Distribution

**Balance work across task stacks**:
- Heavy computation in logic tasks
- Light updates in system tasks
- Render tasks should be lightweight (submit only)

#### 2. Parallel Execution

**Original console optimization**:
- CPU and GPU work in parallel
- Double/triple buffering
- DMA transfers during CPU work

**PC optimization opportunities**:
- Multi-threading potential
- Modern GPU command buffers
- Asynchronous asset loading

#### 3. Frame Pacing

**Smooth frame delivery**:
- Consistent frame times
- Avoid hitches and stutters
- Predictable update intervals

#### 4. Culling and LOD

**Reduce render workload**:
- Frustum culling before submission
- Occlusion culling
- Level of detail (LOD) selection
- Early rejection of invisible objects

### Profiling-Driven Optimization

**Use the profiler to identify**:
1. **Bottleneck phases**: Which stack takes longest?
2. **Heavy tasks**: Which individual tasks are slow?
3. **Frame spikes**: What causes irregular frame times?
4. **GPU stalls**: Where is CPU waiting for GPU?

---

## Common Patterns

### Frame-Based Updates

Many systems update once per frame:

```cpp
void MySystem::Update()
{
    // Called once per frame from a task
    UpdateState();
    ProcessInput();
    UpdatePhysics();
}
```

### Fixed Timestep Physics

Physics often uses **fixed timestep** for determinism:

```cpp
const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
float accumulator = 0.0f;

void PhysicsUpdate(float delta_time)
{
    accumulator += delta_time;
    while (accumulator >= PHYSICS_TIMESTEP)
    {
        PhysicsStep(PHYSICS_TIMESTEP);
        accumulator -= PHYSICS_TIMESTEP;
    }
}
```

### Interpolation

For smooth visuals between physics steps:

```cpp
float alpha = accumulator / PHYSICS_TIMESTEP;
render_position = Lerp(prev_position, curr_position, alpha);
```

---

## Debug Features

### Loop Control

The main loop supports several debug controls:

- **`done` flag**: Exit the loop
- **`display_tasks_paused`**: Freeze rendering
- **`trigger_profiling`**: Start profiling for N frames
- **`currently_profiling`**: Currently profiling flag

### Task Debugging

```cpp
// Dump task list to console
logic_task_stack.Dump();

// Profile task execution
logic_task_stack.Process(true);  // with profiling
```

### Memory Debugging

```cpp
#ifndef __PLAT_WN32__
    MemView_Display();  // Show memory usage
#endif
```

---

## Loop Variants

### Nested Loops

The main loop can be **nested**:

```cpp
void Manager::MainLoop(void)
{
    bool old_flag = done;  // Push current done flag
    done = false;
    
    while (!done)
    {
        // Main loop iteration
    }
    
    done = old_flag;  // Restore previous done flag
}
```

**Use Case**: Modal loops (menus, cutscenes) can run sub-loops

### Pausing

Display tasks can be **paused independently**:

```cpp
display_tasks_paused = true;  // Freeze rendering
// Logic continues, rendering stops
```

**Use Case**: Background loading, pause menus

---

## Integration with Subsystems

### Input System

**Processed in system tasks**:
- Controller polling
- Keyboard/mouse events
- Input buffering
- Input mapping

### Physics System

**Processed in logic tasks**:
- Collision detection
- Rigid body dynamics
- Constraint solving
- Trigger detection

### Rendering System

**Processed in display tasks**:
- Scene graph traversal
- Visibility determination
- Draw call submission
- Material setup

### Audio System

**Processed throughout**:
- System tasks: Audio device management
- Logic tasks: 3D audio position updates
- Display tasks: Audio visualization

### Scripting System

**Processed in logic tasks**:
- Script execution
- Event handling
- Callback invocation
- State machine updates

---

## Best Practices

### Adding New Systems

When adding a new system to the game loop:

1. **Choose the right task stack**:
   - System: Input, network, platform services
   - Logic: Game state, physics, AI
   - Display: Rendering only

2. **Set appropriate priority**:
   - Critical systems: High priority
   - Nice-to-have: Lower priority

3. **Keep tasks lightweight**:
   - Avoid long-running operations
   - Split large work across frames
   - Use timers for periodic updates

4. **Test with profiler**:
   - Measure impact on frame time
   - Identify performance issues early
   - Optimize hot paths

### Common Pitfalls

**❌ Don't**:
- Do heavy computation in display tasks
- Block the main loop
- Assume fixed frame rate
- Ignore frame time spikes

**✅ Do**:
- Profile regularly
- Distribute work evenly
- Handle variable frame times
- Test on target hardware

---

## Related Documentation

- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Overall system architecture
- **[platforms/WIN32.md](platforms/WIN32.md)** - Windows platform specifics
- **[CODE_STYLE.md](CODE_STYLE.md)** - Coding standards
- **[ROADMAP.md](../ROADMAP.md)** - Future improvements

---

## Future Improvements

### Multi-Threading

**Potential improvements**:
- Separate render thread
- Async asset loading
- Job system for parallel tasks
- Task graph dependencies

### Modern Timing

**PC-specific enhancements**:
- High-resolution timers
- Frame pacing with DWM
- G-Sync/FreeSync support
- Unlocked framerate options

### Profiling Tools

**Enhanced debugging**:
- GPU profiler integration
- Per-task memory tracking
- Network latency visualization
- Frame time history graphs

---

## Code Examples

### Basic Task Implementation

```cpp
class MyTask : public Tsk::Task<MyTask>
{
public:
    MyTask() : Task<MyTask>(Tsk::BaseTask::Normal) {}
    
    virtual void Execute()
    {
        // Called once per frame
        UpdateMySystem();
    }
};

// Register with appropriate stack
logic_task_stack.Add(new MyTask());
```

### Conditional Rendering

```cpp
if (!display_tasks_paused)
{
    render_frame();
}
else
{
    // Skip rendering but continue logic
}
```

### Frame Time Measurement

```cpp
uint64 frame_start = Tmr::GetRenderFrame();
// ... frame work ...
uint64 frame_end = Tmr::GetRenderFrame();
float frame_time = (frame_end - frame_start) / 60.0f;  // Assuming 60Hz timer
```

---

## Conclusion

The THUG game loop demonstrates:
- **Clean architecture**: Separate concerns via task stacks
- **Performance focus**: Profiling built-in from the start
- **Platform awareness**: Abstracts platform differences
- **Flexibility**: Supports nested loops, pausing, etc.

Understanding the game loop is essential for:
- Adding new features
- Debugging performance issues
- Porting to new platforms
- Optimizing frame rate

---

## References

- **Source File**: `Code/Gel/MainLoop/Mainloop.cpp`
- **Task System**: `Core/Task/`
- **Timing System**: `Core/Support/Tmr/`
- **Rendering**: `Gfx/NxEngine.cpp`

---

*Last Updated: 2024-10*
*Version: 1.0*
