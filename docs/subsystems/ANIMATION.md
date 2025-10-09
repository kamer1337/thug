# Animation System

## Overview

The THUG animation system provides skeletal animation, blending, and procedural animation for characters and objects. The system is implemented across the Gel and Gfx layers, with the primary component being `CAnimationComponent` in `Code/Gel/Components/`.

## Architecture

### Core Components

#### 1. Animation Component (`Code/Gel/Components/AnimationComponent.h/cpp`)

The `CAnimationComponent` is the main interface for controlling animations on game objects:

```cpp
class CAnimationComponent : public CBaseComponent
{
    // Animation playback control
    uint32 PlayAnim(Script::CStruct *pParams, Script::CScript *pScript, float defaultBlendPeriod = 0.3f);
    void PlaySequence(uint32 checksum, float BlendPeriod = 0.3f);
    
    // Animation queries
    bool AnimEquals(Script::CStruct *pParams, Script::CScript *pScript);
    float AnimDuration(uint32 checksum);
    bool AnimExists(uint32 checksum);
    
    // Time control
    void AddTime(float incVal);
    float GetCurrentAnimTime(void);
    
    // Management
    void SetAnims(uint32 anim_checksum);
    void Reset();
};
```

**Key Features**:
- Animation playback with blending
- Animation sequencing
- Time manipulation (speed up/slow down)
- Script integration for gameplay-driven animations
- Component-based architecture

#### 2. Skeleton Component

Works with the animation system to manage bone hierarchies:

**Responsibilities**:
- Bone hierarchy management
- Pose calculation
- Bone transformations
- Attachment points

#### 3. Model Component

Connects animations to renderable geometry:

**Responsibilities**:
- Apply animations to mesh skinning
- Manage LOD (Level of Detail) for animations
- Handle animation-driven mesh deformation

### Graphics Layer Integration

#### Blend Channels (`Code/Gfx/blendchannel.h`)

Manages animation blending between multiple animation sources:

```cpp
namespace Gfx
{
    class CBlendChannel
    {
        // Blend between multiple animations
        // Weight-based animation mixing
    };
}
```

**Use Cases**:
- Upper/lower body animation separation (e.g., skating while waving)
- Smooth transitions between animations
- Additive animations (e.g., breathing while performing tricks)
- Procedural animation overlay

#### Boned Animation Frame Data (`Gfx::CBonedAnimFrameData`)

Stores per-frame skeletal animation data:

**Contents**:
- Bone positions, rotations, scales
- Frame timing information
- Keyframe interpolation data

#### Pose System (`Gfx::CPose`)

Represents the current state of a skeleton:

**Responsibilities**:
- Store current bone transforms
- Apply animation frames to bones
- Handle bone constraints
- Support IK (Inverse Kinematics) if implemented

#### Procedural Bones (`Gfx::CProceduralBone`)

Allows runtime bone manipulation:

**Use Cases**:
- Head tracking
- Look-at functionality
- Physics-driven bone movement (ragdoll effects)
- Dynamic adjustments to animations

## Animation Assets

### Animation Asset Manager (`Code/Gel/AssMan/animasset.h/cpp`)

Manages loading and caching of animation data:

```cpp
namespace Ass
{
    class CAnimAsset
    {
        // Load animation data from files
        // Cache animations for reuse
        // Handle animation streaming
    };
}
```

**Features**:
- Animation asset loading
- Memory management for animation data
- Animation reference counting
- Streaming support for large animation sets

### Animation Data Storage

**File Formats**:
- Platform-specific animation formats
- Compressed animation data
- Keyframe-based storage

**Memory Management**:
- Animation data can be permanent or temporary
- Streaming system for loading/unloading animations
- Shared animation data across multiple instances

## Animation Blending

### Blend Types

1. **Linear Blending**:
   - Smooth transition from one animation to another
   - Controlled by blend period parameter
   - Default blend period: 0.3 seconds

2. **Weight-Based Blending**:
   - Multiple animations with different weights
   - Used for partial body animations
   - Supports additive blending

3. **Layer-Based Blending**:
   - Stack multiple animations
   - Higher layers can override lower layers
   - Used for independent body part control

### Blend Channels

Animation channels allow different parts of the skeleton to play different animations:

**Examples**:
- **Upper Body**: Waving animation
- **Lower Body**: Skateboarding animation
- **Head**: Look-at target

This enables complex, layered animation behaviors.

## Animation Sequences

### Sequence Playback

Sequences allow chaining multiple animations:

```cpp
// Play a sequence of animations
animComponent->PlaySequence(CRCD("TrickComboSequence"), 0.2f);
```

**Sequence Features**:
- Automatic transition between animations
- Configurable blend times between sequence steps
- Loop control
- Event callbacks at sequence points

### Animation Events

Animations can trigger events at specific frames:

**Event Types**:
- Sound effects (footsteps, impacts)
- Particle effects (dust, sparks)
- Gameplay events (trick completion, landing detection)
- Camera shake
- Controller vibration

**Event Table**: `m_animEventTableName` stores the checksum of the event table

## Script Integration

### Script-Driven Animation

The animation system is deeply integrated with the game's scripting system:

```cpp
// From scripts, animations can be controlled
EMemberFunctionResult CallMemberFunction(uint32 Checksum, 
                                        Script::CStruct* pParams, 
                                        Script::CScript* pScript);
```

**Script Functions**:
- `PlayAnim`: Play a specific animation with parameters
- `AnimEquals`: Check if a specific animation is playing
- `SetAnims`: Load a new animation set
- `Reset`: Reset animation state

### Waiting for Animations

Scripts can wait for animations to complete:

```cpp
void ProcessWait(Script::CScript * pScript);
```

This enables synchronized gameplay and animation scripting.

## Animation System Update

### Update Loop

The animation component updates during the game loop:

```cpp
virtual void Update();
virtual void Finalize();
```

**Update Process**:
1. Advance animation time
2. Evaluate current frame/blend state
3. Update blend channels
4. Calculate final pose
5. Apply pose to skeleton
6. Trigger any animation events
7. Check for animation completion

**Timing**:
- Updates occur during the component update phase
- Frame-rate independent animation playback
- Time dilation support (slow motion, fast forward)

## Skateboarding-Specific Animations

### Trick Animations

Special handling for skateboarding tricks:

**Features**:
- Rotation-based animation selection
- Speed-based animation blending
- Landing animation transitions
- Bail animations

### Balance Animations

Dynamic animations for balance mechanics:

**Use Cases**:
- Manual balance tilting
- Grind balance adjustments
- Lip trick balancing

### Procedural Adjustments

Runtime animation modifications:

**Examples**:
- Lean angle based on turning
- Height adjustments for terrain
- Speed-based animation playback rate

## Performance Considerations

### Optimization Techniques

1. **Animation Culling**:
   - Don't update animations for off-screen objects
   - Use simplified animations at distance

2. **LOD (Level of Detail)**:
   - Reduce animation update rate for distant characters
   - Use fewer blend channels at distance
   - Simplify bone hierarchies for background characters

3. **Animation Caching**:
   - Cache frequently used animation data
   - Share animation data across instances
   - Preload common animations

4. **Blend Optimization**:
   - Limit number of active blend channels
   - Use cheaper blending methods when possible
   - Early-out for zero-weight blends

### Memory Management

- **Streaming**: Load/unload animations as needed
- **Compression**: Store animations in compressed format
- **Sharing**: Multiple objects can reference the same animation data

## Key Files

### Gel Layer
- `Code/Gel/Components/animationcomponent.h/cpp` - Main animation component
- `Code/Gel/AssMan/animasset.h/cpp` - Animation asset management

### Gfx Layer
- `Code/Gfx/blendchannel.h` - Blend channel system
- Platform-specific animation rendering code

### Sk Layer
- Skateboarding-specific animation handling
- Trick animation management

## Usage Examples

### Playing a Simple Animation

```cpp
// Get animation component
CAnimationComponent* anim = GetAnimationComponent();

// Play animation with default blend
Script::CStruct params;
params.AddChecksum("anim", CRCD("Trick_Kickflip"));
anim->PlayAnim(&params, nullptr, 0.3f);
```

### Checking Animation State

```cpp
// Check if specific animation is playing
if (anim->AnimExists(CRCD("Trick_Kickflip")))
{
    float duration = anim->AnimDuration(CRCD("Trick_Kickflip"));
    float current_time = anim->GetCurrentAnimTime();
    
    // Calculate progress
    float progress = current_time / duration;
}
```

### Time Manipulation

```cpp
// Speed up animation (fast motion)
anim->AddTime(deltaTime * 2.0f);

// Slow down animation (slow motion)
anim->AddTime(deltaTime * 0.5f);
```

## Future Enhancements

### Potential Improvements

1. **Modern Animation Systems**:
   - Integration with animation middleware (e.g., Granny, RAD)
   - State machine-based animation
   - Animation graphs for complex blending

2. **Advanced Blending**:
   - Improved blend algorithms (cubic, spherical)
   - Cross-fade optimization
   - Blend masks for partial body control

3. **Procedural Animation**:
   - Full IK system
   - Physics-based secondary motion
   - Muscle simulation

4. **Debugging Tools**:
   - Visual skeleton display
   - Animation timeline viewer
   - Blend weight visualization
   - Performance profiling per animation

5. **Asset Pipeline**:
   - Modern animation import tools
   - Animation compression tools
   - Batch animation processing

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall system architecture
- [GAME_LOOP.md](../GAME_LOOP.md) - Animation update timing
- [RENDERING.md](RENDERING.md) - How animations affect rendering (when created)

## Notes

The animation system is tailored for the skateboarding genre with specific optimizations for:
- Fast animation transitions (tricks happen quickly)
- Layered animations (upper/lower body independence)
- Script-driven gameplay animations
- Performance on original console hardware

When porting to modern platforms:
- Consider using modern animation middleware
- Add animation compression/streaming for larger animation sets
- Implement more sophisticated blend systems
- Add debugging and visualization tools
- Create unit tests for animation blending calculations
