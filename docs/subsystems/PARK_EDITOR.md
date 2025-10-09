# THUG Park Editor System

## Overview

Tony Hawk's Underground features a powerful Park Editor (also known as Create-A-Park or CAP) that allows players to design custom skateparks. The system provides an intuitive grid-based interface for placing ramps, rails, structures, and decorative objects to create unique skateable environments.

## Table of Contents

- [Architecture](#architecture)
- [Core Components](#core-components)
- [Piece System](#piece-system)
- [Grid and Placement](#grid-and-placement)
- [Editor States](#editor-states)
- [Data Structures](#data-structures)
- [Save/Load System](#saveload-system)
- [Rendering and Preview](#rendering-and-preview)

---

## Architecture

### System Structure

The Park Editor is implemented in two main modules:

```
┌─────────────────────────────────────────────────────┐
│         Ed::CParkEditor (Main Module)               │
│      Code/Sk/ParkEditor2/ParkEd.h/cpp               │
│    - Editor state management                        │
│    - User input handling                            │
│    - Camera control                                 │
│    - Save/load operations                           │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│       Ed::CParkGenerator (Geometry Builder)         │
│      Code/Sk/ParkEditor2/ParkGen.h/cpp              │
│    - Piece placement and management                 │
│    - Grid-based positioning                         │
│    - Geometry generation                            │
│    - Collision building                             │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│         Ed::CEdMap (Map Manager)                    │
│      Code/Sk/ParkEditor2/EdMap.h/cpp                │
│    - Meta-piece management                          │
│    - Piece composition                              │
│    - Gap tracking                                   │
└─────────────────────────────────────────────────────┘
```

### Design Philosophy

1. **Grid-Based Placement**: All pieces snap to a 3D grid
2. **Modular Pieces**: Combine small parts into complex structures
3. **Real-Time Preview**: Immediate visual feedback
4. **Memory Efficiency**: Pool-based allocation, streaming
5. **Accessibility**: User-friendly for all skill levels

---

## Core Components

### CParkEditor Class

**File**: `Code/Sk/ParkEditor2/ParkEd.h`

Main editor module managing the overall editing experience:

```cpp
class CParkEditor : public Mdl::Module, public Obj::CEventListener 
{
public:
    enum EEditorState
    {
        vINACTIVE,          // Editor not active
        vEDITING,           // Actively editing
        vTEST_PLAY,         // Testing park in-game
        vREGULAR_PLAY,      // Playing completed park
    };
    
    void Initialize(bool edit_mode);
    void Rebuild(bool justRebuildFloor, bool clearMap, bool makeCursor);
    void SetState(EEditorState desiredState);
    void Update();
    
    void AccessDisk(bool save, int slot, bool blockRebuild);
    void Cleanup();
    
    bool UsingCustomPark();
    bool EditingCustomPark();
};
```

### CParkGenerator Class

**File**: `Code/Sk/ParkEditor2/ParkGen.h`

Handles piece placement, geometry generation, and park building:

```cpp
class CParkGenerator
{
public:
    void AddPiece(CClonedPiece* pPiece);
    void RemovePiece(CClonedPiece* pPiece);
    void MovePiece(CClonedPiece* pPiece, GridDims newPos);
    
    void GenerateGeometry();
    void BuildCollision();
    void UpdateSuperSectors();
    
    CClonedPiece* GetPieceAt(GridDims pos);
    bool CanPlacePiece(GridDims pos, CSourcePiece* pSource);
};
```

### CEdMap Class

**File**: `Code/Sk/ParkEditor2/EdMap.h`

Manages the editor map, meta-pieces, and piece relationships:

```cpp
class CEdMap
{
public:
    void AddMetaPiece(CMetaPiece* pMeta);
    void RemoveMetaPiece(CMetaPiece* pMeta);
    
    CMetaPiece* GetMetaPieceAt(GridDims pos);
    void UpdateGaps();
    void ClearMap();
};
```

---

## Piece System

### Piece Hierarchy

The park editor uses a hierarchical piece system:

```
CPiece (Base)
  ├── CSourcePiece
  │     - Template/master piece
  │     - Dimensions and properties
  │     - Mesh data reference
  │
  └── CClonedPiece
        - Instance in the park
        - Position and rotation
        - Reference to source piece
```

### CSourcePiece

**Definition**: Template piece with dimensions and mesh data

```cpp
class CSourcePiece : public CPiece
{
public:
    GridDims GetDims() const;       // Piece dimensions in grid cells
    uint32 GetNameChecksum() const; // Unique identifier
    Nx::CModel* GetModel() const;   // 3D mesh
    
    bool IsRamp() const;
    bool IsRail() const;
    bool IsStruct() const;
};
```

**Piece Categories**:
- **Ramps**: Quarter pipes, half pipes, bowls, spines
- **Rails**: Straight rails, curved rails, ledges
- **Structures**: Platforms, stairs, pyramids
- **Decorations**: Benches, lights, trees, signs
- **Floor**: Ground tiles, textures, gaps

### CClonedPiece

**Definition**: Instance of a source piece placed in the park

```cpp
class CClonedPiece : public CPiece
{
public:
    GridDims GetPos() const;            // Position in grid
    uint8 GetRotation() const;          // Rotation (0, 90, 180, 270)
    CSourcePiece* GetSource() const;    // Template reference
    
    void SetPos(GridDims pos);
    void SetRotation(uint8 rot);
    void UpdateGeometry();
};
```

### CMetaPiece

**Definition**: Group of pieces treated as a single unit

**File**: `Code/Sk/ParkEditor2/EdMap.h`

```cpp
class CMetaPiece
{
public:
    enum EFlags
    {
        mCONCRETE_META,     // Instantiated in park
        mABSTRACT_META,     // Template definition
        mIN_PARK,           // Placed in park
        mIS_RISER,          // Height-adjusting piece
        mUSER_CREATED,      // Custom meta-piece
    };
    
    void AddDescriptor(SMetaDescriptor desc);
    int GetNumDescriptors() const;
    GridDims GetDims() const;
    
    void Generate();        // Create cloned pieces
    void Destroy();         // Remove from park
};
```

**Meta-Piece Types**:

1. **Concrete Meta-Piece**: Actual pieces in the park
2. **Abstract Meta-Piece**: Template for creating concrete instances
3. **User-Created Meta-Piece**: Player-defined combinations

**Meta-Piece Descriptor**:

```cpp
struct SMetaDescriptor
{
    uint8 mX, mY, mZ;       // Position (relative)
    uint8 mRot;             // Rotation
    uint8 mRiser;           // Height modifier flag
    
    union {
        uint32 mPieceName;  // Name (abstract)
        CPiece* mpPiece;    // Pointer (concrete)
    };
};
```

---

## Grid and Placement

### Grid System

The park uses a 3D grid coordinate system:

**File**: `Code/Sk/ParkEditor2/ParkGen.h`

```cpp
class GridDims
{
public:
    uint8 GetX() const;     // X coordinate (horizontal)
    sint8 GetY() const;     // Y coordinate (vertical/height)
    uint8 GetZ() const;     // Z coordinate (depth)
    
    uint8 GetW() const;     // Width (in cells)
    uint8 GetH() const;     // Height (in cells)
    uint8 GetL() const;     // Length (in cells)
    
    void SetXYZ(uint8 x, sint8 y, uint8 z);
    void SetWHL(uint8 w, uint8 h, uint8 l);
};
```

**Grid Properties**:
- **Cell Size**: Configurable (typically 8-12 feet)
- **Grid Dimensions**: 32x32 cells (horizontal)
- **Height Levels**: Multiple vertical layers
- **Snapping**: Pieces snap to grid intersections

### Placement Rules

**Collision Detection**:
```cpp
bool CanPlacePiece(GridDims pos, CSourcePiece* pSource)
{
    // Check bounds
    if (OutOfBounds(pos, pSource->GetDims()))
        return false;
    
    // Check for overlapping pieces
    if (PieceExistsAt(pos, pSource->GetDims()))
        return false;
    
    // Check for support (pieces need ground or platform below)
    if (!HasSupport(pos, pSource))
        return false;
    
    return true;
}
```

**Rotation**:
- Pieces rotate in 90° increments
- Rotation affects dimensions (W/L swap)
- Rotation center is piece origin

**Height Adjustment**:
- Risers add vertical offset
- Pieces can be placed at different Y levels
- Height affects skating physics

---

## Editor States

### State Machine

The editor operates in distinct states:

```cpp
enum EEditorState
{
    vINACTIVE,          // Not using editor
    vEDITING,           // Active editing
    vTEST_PLAY,         // Testing the park
    vREGULAR_PLAY,      // Playing final park
};
```

### State Transitions

```
┌─────────────┐
│  vINACTIVE  │ ◄──────────────────┐
└─────┬───────┘                    │
      │ Initialize()               │
      ↓                            │
┌─────────────┐                    │
│  vEDITING   │ ◄──────┐           │
└─────┬───────┘        │           │
      │ TestPark()     │           │
      ↓                │           │
┌─────────────┐        │           │
│ vTEST_PLAY  │────────┘           │
└─────┬───────┘  ReturnToEditor()  │
      │                            │
      │ FinishPark()               │
      ↓                            │
┌─────────────┐                    │
│vREGULAR_PLAY│────────────────────┘
└─────────────┘      Cleanup()
```

### State Behaviors

**vEDITING**:
- Cursor visible and controllable
- Pieces can be added/removed
- Camera in overhead/free mode
- UI shows piece menu and tools

**vTEST_PLAY**:
- Full game controls active
- Editor cursor hidden
- Park collision enabled
- Can skate and test tricks

**vREGULAR_PLAY**:
- Normal gameplay mode
- Custom park fully integrated
- No editor access
- Park saved to memory card

---

## Data Structures

### Park Map

The editor maintains a 3D map of placed pieces:

```cpp
// Simplified map structure
class CParkMap
{
private:
    CClonedPiece* m_grid[32][16][32];  // X, Y, Z
    
public:
    CClonedPiece* GetPieceAt(int x, int y, int z);
    void SetPieceAt(int x, int y, int z, CClonedPiece* p);
    void Clear();
};
```

### Clipboard System

**File**: `Code/Sk/ParkEditor2/clipboard.h`

Players can copy and paste groups of pieces:

```cpp
class CClipboard
{
public:
    void Copy(CMetaPiece* pMeta);
    void Paste(GridDims pos);
    void Clear();
    
    bool HasContent() const;
    CMetaPiece* GetContent() const;
};
```

### Gap Manager

**File**: `Code/Sk/ParkEditor2/GapManager.h`

Manages skateable gaps (transitions between pieces):

```cpp
class CGapManager
{
public:
    void AddGap(GridDims start, GridDims end);
    void RemoveGap(GridDims pos);
    void UpdateGaps();
    
    bool IsGapAt(GridDims pos) const;
    void GetGapList(Script::CArray* pArray);
};
```

**Gap Types**:
- Spine gaps (between ramps)
- Rail gaps (over obstacles)
- Manual gaps (flatground links)
- Lip gaps (edge transfers)

---

## Save/Load System

### Memory Card Format

Parks are saved to memory card in a compact binary format:

**Save Structure**:
```
┌─────────────────────────────────┐
│ Header                          │
│  - Version number               │
│  - Theme ID                     │
│  - Park name                    │
│  - Dimensions                   │
├─────────────────────────────────┤
│ Piece Data                      │
│  - Piece count                  │
│  - Piece entries (compressed)   │
│    * Source piece ID            │
│    * Position (X, Y, Z)         │
│    * Rotation                   │
├─────────────────────────────────┤
│ Meta-Piece Data                 │
│  - Meta-piece count             │
│  - Meta-piece definitions       │
├─────────────────────────────────┤
│ Gap Data                        │
│  - Gap count                    │
│  - Gap definitions              │
├─────────────────────────────────┤
│ Footer                          │
│  - Checksum                     │
│  - Thumbnail (optional)         │
└─────────────────────────────────┘
```

### Save/Load Operations

```cpp
// Save park to memory card
void CParkEditor::AccessDisk(bool save, int slot, bool blockRebuild)
{
    if (save)
    {
        // Serialize park data
        uint8* pBuffer = SerializePark();
        
        // Write to memory card
        MemCard::WriteFile(slot, "park.dat", pBuffer, size);
        
        // Generate thumbnail
        SaveThumbnail(slot);
    }
    else
    {
        // Load from memory card
        uint8* pBuffer = MemCard::ReadFile(slot, "park.dat");
        
        // Deserialize park data
        PostMemoryCardLoad(pBuffer, oldTheme);
        
        // Rebuild geometry
        if (!blockRebuild)
            Rebuild(false, false, true);
    }
}
```

### Park Regeneration

After loading, the park must be regenerated:

```cpp
void CParkEditor::Rebuild(bool justRebuildFloor, 
                          bool clearMap, 
                          bool makeCursor)
{
    if (clearMap)
    {
        // Clear existing park
        ClearAllPieces();
    }
    
    if (justRebuildFloor)
    {
        // Rebuild ground only
        BuildFloor();
    }
    else
    {
        // Full rebuild
        RegenerateFromMap();
        GenerateGeometry();
        BuildCollision();
        UpdateSuperSectors();
    }
    
    if (makeCursor)
    {
        // Create editor cursor
        CreateCursor();
    }
}
```

---

## Rendering and Preview

### Editor Camera

**File**: `Code/Sk/Components/EditorCameraComponent.h`

The editor uses a specialized camera:

```cpp
class CEditorCameraComponent : public CBaseComponent
{
public:
    void SetMode(ECameraMode mode);
    void Update();
    
    enum ECameraMode
    {
        TOP_DOWN,       // Overhead view
        FREE_CAM,       // Free movement
        PREVIEW,        // Cinematic preview
    };
};
```

**Camera Controls**:
- **Top-Down**: Fixed overhead for placement
- **Free-Cam**: 6DOF for inspection
- **Preview**: Automated park tour

### Cursor Rendering

The editor cursor shows the selected piece:

```cpp
class CCursor
{
public:
    void SetPiece(CSourcePiece* pSource);
    void SetPosition(GridDims pos);
    void SetRotation(uint8 rot);
    void Render();
    
    bool IsPlaceable() const;  // Valid placement location
};
```

**Cursor States**:
- **Green**: Can place piece here
- **Red**: Invalid placement
- **Yellow**: Partial collision
- **Blue**: Selection mode

### Real-Time Updates

The editor provides immediate visual feedback:

1. **Cursor Update**: Every frame
2. **Geometry Rebuild**: On piece add/remove
3. **Collision Update**: After geometry rebuild
4. **Super Sector Update**: After collision update

### Performance Considerations

**Memory Management**:
```cpp
// Memory margins for editing
#define TOP_DOWN_REQUIRED_MARGIN 1000000
#define TOP_DOWN_REQUIRED_MARGIN_LEEWAY 2000
```

The editor reserves memory for:
- Temporary geometry buffers
- Undo/redo history
- Clipboard storage
- Super sector rebuilding

**Optimization Strategies**:
- **Piece Pooling**: Reuse piece instances
- **Deferred Updates**: Batch geometry rebuilds
- **LOD Selection**: Lower detail when editing
- **Culling**: Don't render hidden pieces

---

## Usage Examples

### Creating a Simple Park

```cpp
// Initialize editor
CParkEditor* pEditor = CParkEditor::Instance();
pEditor->Initialize(true);  // edit_mode = true
pEditor->SetState(CParkEditor::vEDITING);

// Get park generator
CParkGenerator* pGen = GetParkGenerator();

// Add a quarter pipe
CSourcePiece* pQP = GetPieceByName("QP_8x8");
GridDims pos(10, 0, 10);  // X, Y, Z
CClonedPiece* pClone = pGen->CreateClonedPiece(pQP, pos, 0);
pGen->AddPiece(pClone);

// Add a rail
CSourcePiece* pRail = GetPieceByName("Rail_Straight_8");
GridDims railPos(10, 0, 15);
CClonedPiece* pRailClone = pGen->CreateClonedPiece(pRail, railPos, 90);
pGen->AddPiece(pRailClone);

// Rebuild park
pEditor->Rebuild(false, false, true);

// Test the park
pEditor->SetState(CParkEditor::vTEST_PLAY);
```

### Creating a Meta-Piece

```cpp
// Create abstract meta-piece (template)
CAbstractMetaPiece* pMeta = new CAbstractMetaPiece();
pMeta->SetName("MyRamp");

// Add descriptors
SMetaDescriptor desc1;
desc1.mX = 0; desc1.mY = 0; desc1.mZ = 0;
desc1.mRot = 0;
desc1.mPieceName = CRCD("QP_8x8");
pMeta->AddDescriptor(desc1);

SMetaDescriptor desc2;
desc2.mX = 8; desc2.mY = 0; desc2.mZ = 0;
desc2.mRot = 180;
desc2.mPieceName = CRCD("QP_8x8");
pMeta->AddDescriptor(desc2);

// Register meta-piece
RegisterMetaPiece(pMeta);

// Now can place "MyRamp" as a single unit
```

---

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall engine architecture
- [RENDERING.md](../RENDERING.md) - Graphics rendering system
- [docs/CODE_STYLE.md](../CODE_STYLE.md) - Coding conventions

## Implementation Files

### Core Editor
- `Code/Sk/ParkEditor2/ParkEd.h/cpp` - Main editor module (8,000+ lines)
- `Code/Sk/ParkEditor2/ParkGen.h/cpp` - Geometry generator (4,000+ lines)
- `Code/Sk/ParkEditor2/EdMap.h/cpp` - Map management (1,300+ lines)
- `Code/Sk/ParkEditor2/clipboard.h/cpp` - Copy/paste system
- `Code/Sk/ParkEditor2/GapManager.h/cpp` - Gap management

### Components
- `Code/Sk/Components/EditorCameraComponent.h/cpp` - Editor camera
- `Code/Sk/Components/RailEditorComponent.h/cpp` - Rail editing
- `Code/Sk/Components/GoalEditorComponent.h/cpp` - Goal editing

### Legacy
- `Code/Sk/ParkEditor/` - Original park editor (older version)

---

*This document describes the Park Editor system in Tony Hawk's Underground.*
