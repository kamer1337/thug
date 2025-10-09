# Physics and Collision Detection System

## Overview

The THUG physics system handles collision detection, skateboard physics, and world interaction. The system is primarily implemented in the `Code/Sk/Engine/` directory and integrates with the Gel collision system.

## Architecture

### Core Components

#### 1. Feeler System (`Code/Sk/Engine/feeler.h/cpp`)

The feeler system provides ray-casting and line-based collision detection:

```cpp
class CFeeler : public Mth::Line
{
    // Ray-cast collision detection
    bool GetCollision(const Mth::Vector &start, const Mth::Vector &end, bool movables = true);
    
    // Returns collision information
    float GetDist() const;              // Distance to collision point
    const Mth::Vector& GetPoint() const; // Collision point
    const Mth::Vector& GetNormal() const; // Surface normal
    ETerrainType GetTerrain() const;    // Surface material type
};
```

**Key Features**:
- Line-based collision queries
- Support for static and movable objects
- Terrain type detection (concrete, grass, metal, etc.)
- Callback system for custom collision handling
- Brightness sampling for lighting calculations

**Usage**:
- Skateboard ground detection
- Wall riding detection
- Rail grinding detection
- General object collision

#### 2. Rectangle Feeler (`Code/Sk/Engine/RectFeeler.h/cpp`)

Extended feeler for area-based collision detection:

```cpp
class CRectFeeler
{
    // Rectangle-based collision detection
    // Used for wider area checks
};
```

**Use Cases**:
- Skateboard deck collision
- Wider surface detection
- Area-based terrain queries

#### 3. Super Sector System (`Code/Sk/Engine/SuperSector.h/cpp`)

Spatial partitioning and collision optimization:

**Purpose**:
- Divide the world into manageable collision sectors
- Optimize collision queries by limiting search space
- Manage static and dynamic collision geometry

#### 4. Contact System (`Code/Sk/Engine/contact.h/cpp`)

Manages collision contact points and resolution:

**Responsibilities**:
- Track collision contact points
- Calculate collision response
- Manage friction and bouncing

### Integration with Gel Collision System

The physics system integrates with `Code/Gel/Collision/` for:

- **Collision Cache** (`Nx::CCollCache`): Cache collision queries for performance
- **Collision Objects** (`Nx::CCollObj`): Represent collidable geometry
- **Collision Flags**: Define surface properties and behaviors

## Skateboard Physics

### Movement Physics

The skateboard physics system handles:

1. **Ground Physics**:
   - Speed calculation based on slope
   - Friction simulation
   - Terrain-specific physics (rough/smooth surfaces)

2. **Air Physics**:
   - Gravity simulation
   - Jump arc calculation
   - Air control and rotation

3. **Trick Physics**:
   - Angular momentum
   - Spin rate calculations
   - Landing impact detection

### Special Movement Modes

#### Wall Riding
- Surface angle detection
- Gravity orientation changes
- Speed maintenance calculations

#### Rail Grinding
- Rail detection and alignment
- Balance simulation
- Grind speed calculation

#### Manual/Nose Manual
- Balance physics
- Pivot point calculations
- Stability simulation

## Collision Detection Pipeline

### 1. Broad Phase
- Super sector queries to find potential colliders
- Bounding box/sphere tests

### 2. Narrow Phase
- Precise ray/geometry intersection tests
- Contact point generation

### 3. Response Phase
- Calculate collision normal and penetration depth
- Apply impulses for separation
- Calculate friction and sliding

## Terrain Types

The system supports multiple terrain types (from `gel/collision/collision.h`):

```cpp
enum ETerrainType
{
    TERRAIN_CONCRETE,
    TERRAIN_GRASS,
    TERRAIN_METAL,
    TERRAIN_WOOD,
    // ... etc
};
```

**Impact on Physics**:
- Different friction values
- Different sound effects
- Different particle effects
- Speed modifiers

## Performance Considerations

### Optimization Techniques

1. **Spatial Partitioning**: SuperSector system divides world into manageable chunks
2. **Collision Caching**: Results are cached to avoid redundant calculations
3. **Level of Detail**: Distant objects use simplified collision
4. **Selective Testing**: Only test relevant collision types per query

### Performance Tips

- Use cached feelers when performing multiple similar queries
- Limit movable collision checks to nearby objects
- Use coarse tests (bounding volumes) before precise tests
- Batch collision queries when possible

## Key Files

### Sk/Engine Directory
- `feeler.h/cpp` - Line-based collision detection
- `RectFeeler.h/cpp` - Rectangle-based collision
- `SuperSector.h/cpp` - Spatial partitioning
- `contact.h/cpp` - Contact point management
- `sounds.h/cpp` - Physics-based sound triggering

### Gel/Collision Directory
- `collision.h/cpp` - Core collision definitions
- Platform-specific collision implementations

## Usage Examples

### Basic Ground Detection

```cpp
CFeeler feeler;
feeler.SetLine(skater_pos, skater_pos + Mth::Vector(0, -100, 0));

if (feeler.GetCollision())
{
    float ground_dist = feeler.GetDist();
    Mth::Vector ground_normal = feeler.GetNormal();
    ETerrainType terrain = feeler.GetTerrain();
    
    // Apply physics based on terrain type
}
```

### Wall Riding Detection

```cpp
CFeeler wall_feeler;
wall_feeler.SetLine(skater_pos, skater_pos + forward_direction * 50);

if (wall_feeler.GetCollision())
{
    Mth::Vector wall_normal = wall_feeler.GetNormal();
    float angle = CalculateAngle(wall_normal, up_vector);
    
    if (angle > WALL_RIDE_MIN_ANGLE && angle < WALL_RIDE_MAX_ANGLE)
    {
        // Enter wall ride mode
    }
}
```

## Future Enhancements

### Potential Improvements

1. **Modern Physics Integration**:
   - Consider integrating modern physics libraries (Bullet, PhysX)
   - Maintain skateboard-specific behavior while improving general physics

2. **Better Collision Shapes**:
   - Add support for more complex collision primitives
   - Implement convex hull collision for complex objects

3. **Performance Profiling**:
   - Add instrumentation to identify bottlenecks
   - Profile collision queries per frame

4. **Enhanced Debugging**:
   - Visual debugging tools for collision geometry
   - Real-time feeler visualization
   - Collision query statistics

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall system architecture
- [GAME_LOOP.md](../GAME_LOOP.md) - Where physics updates occur in the frame
- [Win32 Stub Functions](../platforms/STUB_FUNCTIONS.md) - Platform-specific collision implementations

## References

### Code Locations

- **Physics Engine**: `Code/Sk/Engine/`
- **Collision System**: `Code/Gel/Collision/`
- **Math Library**: `Code/Core/Math/`
- **Skater Object**: `Code/Sk/Objects/skater.cpp`

### Related Systems

- **Animation System**: Physics drives animation blending
- **Sound System**: Collision events trigger sounds
- **Particle System**: Collision events trigger particle effects
- **Camera System**: Physics influences camera behavior

## Notes

This is legacy code from the original THUG game. The physics system is tailored specifically for skateboarding gameplay and may not be suitable as a general-purpose physics engine. The system has been optimized for the specific needs of the game and the constraints of the original target platforms (PS2, Xbox, GameCube).

For modern platform ports, consider:
- Abstracting the collision interface for easier backend replacement
- Adding unit tests for physics calculations
- Documenting the specific skateboard physics parameters and tuning values
- Creating a compatibility layer if integrating modern physics libraries
