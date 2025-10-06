# THUG2 Technical Implementation Guide

## Overview

This document provides detailed technical guidance for implementing THUG2 features on top of the THUG codebase. It includes specific code patterns, API extensions, and implementation strategies based on analysis of the THUG source code architecture.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Freak Out System Implementation](#freak-out-system-implementation)
3. [Tag Graffiti System](#tag-graffiti-system)
4. [World Destruction System](#world-destruction-system)
5. [Sticker Slap Mechanics](#sticker-slap-mechanics)
6. [World Destruction Tour Mode](#world-destruction-tour-mode)
7. [Script Extensions](#script-extensions)
8. [Network Protocol Changes](#network-protocol-changes)
9. [Asset Pipeline Modifications](#asset-pipeline-modifications)
10. [Performance Optimization](#performance-optimization)

---

## Architecture Overview

### Component-Based Design

THUG uses a component-based architecture for game objects:

```cpp
// Base component system (from Code/Gel/Components/)
class CBaseComponent : public Obj::CObject
{
public:
    virtual void Update();
    virtual void InitFromStructure(Script::CStruct* pParams);
    virtual void RefreshFromStructure(Script::CStruct* pParams);
};
```

### Extending for THUG2

THUG2 features can be added as new components or extensions to existing components:

```cpp
// Example: New components for THUG2
// Code/Sk/Components/SkaterGraffitiComponent.h
// Code/Sk/Components/SkaterFreakOutComponent.h
// Code/Sk/Components/DestructibleComponent.h
```

---

## Freak Out System Implementation

### Overview

Freak Out is an enhanced special meter mode in THUG2 that provides unlimited special moves for a limited time.

### Integration Points

#### 1. Skater State Extension

**File**: `Code/Sk/Objects/skater.h`, `Code/Sk/Objects/skater.cpp`

```cpp
class CSkater : public CCompositeObject
{
public:
    // Existing THUG special meter
    float GetSpecial() const { return m_special; }
    void SetSpecial(float special) { m_special = special; }
    
    // THUG2: Freak Out additions
    bool IsInFreakOutMode() const { return m_freak_out_active; }
    void ActivateFreakOut();
    void DeactivateFreakOut();
    void UpdateFreakOut(float delta_time);
    
private:
    // Existing
    float m_special;
    
    // THUG2 additions
    bool m_freak_out_active;
    float m_freak_out_timer;
    float m_freak_out_duration;  // Default: 10-15 seconds
};
```

#### 2. Implementation

```cpp
// Code/Sk/Objects/skater.cpp

void CSkater::ActivateFreakOut()
{
    if (m_special >= 1.0f)  // Full special meter required
    {
        m_freak_out_active = true;
        m_freak_out_timer = m_freak_out_duration;
        
        // Visual feedback
        StartFreakOutEffect();
        
        // Audio feedback
        PlayFreakOutSound();
        
        // Notify other systems
        Mdl::Skate* pSkate = Mdl::Skate::Instance();
        if (pSkate)
        {
            pSkate->OnFreakOutActivated(this);
        }
    }
}

void CSkater::UpdateFreakOut(float delta_time)
{
    if (!m_freak_out_active) return;
    
    m_freak_out_timer -= delta_time;
    
    if (m_freak_out_timer <= 0.0f)
    {
        DeactivateFreakOut();
    }
    
    // Keep special meter full during Freak Out
    m_special = 1.0f;
}

void CSkater::DeactivateFreakOut()
{
    m_freak_out_active = false;
    m_freak_out_timer = 0.0f;
    m_special = 0.0f;  // Reset special meter
    
    // Stop visual effects
    StopFreakOutEffect();
}
```

#### 3. Trick System Integration

**File**: `Code/Sk/Objects/SkaterTricks.h`

```cpp
// Modify trick execution to check Freak Out state
bool CSkater::CanDoSpecialTrick() const
{
    // THUG: Only when special meter is full
    // return m_special >= 1.0f;
    
    // THUG2: Also during Freak Out
    return m_special >= 1.0f || m_freak_out_active;
}

// Modify trick scoring
int CSkater::CalculateTrickScore(uint32 trick_id)
{
    int base_score = GetTrickBaseScore(trick_id);
    
    // THUG2: Bonus multiplier during Freak Out
    if (m_freak_out_active)
    {
        base_score *= 2;  // Double points
    }
    
    return base_score;
}
```

#### 4. UI Integration

**File**: `Code/Gfx/2D/` (HUD system)

```cpp
// Add Freak Out indicator to HUD
void DrawSpecialMeter()
{
    // Existing special meter drawing
    DrawSpecialMeterBar(m_special);
    
    // THUG2: Freak Out mode indicator
    if (pSkater->IsInFreakOutMode())
    {
        DrawFreakOutTimer(pSkater->GetFreakOutTimeRemaining());
        DrawFreakOutEffect();  // Pulsing/glowing effect
    }
}
```

### Script Commands

Add QB script commands for Freak Out:

```cpp
// Code/Gel/Scripting/ - Register new commands

bool ScriptActivateFreakOut(Script::CStruct* pParams, Script::CScript* pScript)
{
    Obj::CSkater* pSkater = GetScriptedSkater();
    if (pSkater)
    {
        pSkater->ActivateFreakOut();
        return true;
    }
    return false;
}

bool ScriptIsFreakOutActive(Script::CStruct* pParams, Script::CScript* pScript)
{
    Obj::CSkater* pSkater = GetScriptedSkater();
    if (pSkater)
    {
        return pSkater->IsInFreakOutMode();
    }
    return false;
}
```

QB Script usage:
```
if ObjectInFreakOutMode
    // Do something special
endif

ActivateFreakOut
```

---

## Tag Graffiti System

### Overview

Tag graffiti allows players to spray paint tags on surfaces throughout levels.

### Component Architecture

#### 1. Graffiti Component

**New File**: `Code/Sk/Components/SkaterGraffitiComponent.h`

```cpp
namespace Obj
{
    class CSkaterGraffitiComponent : public CBaseComponent
    {
        DeclareSkaComponentClass(CSkaterGraffitiComponent);
        
    public:
        CSkaterGraffitiComponent();
        virtual ~CSkaterGraffitiComponent();
        
        virtual void Update();
        virtual void InitFromStructure(Script::CStruct* pParams);
        
        // Graffiti operations
        bool CanSprayTag() const;
        bool SprayTag();
        void AddTags(int count);
        int GetRemainingTags() const { return m_tags_remaining; }
        
        // Tag tracking
        uint32 GetTotalTagsPlaced() const { return m_total_tags_placed; }
        bool HasTaggedSurface(uint32 surface_id) const;
        
    private:
        // Tag inventory
        int m_tags_remaining;
        int m_max_tags;
        uint32 m_total_tags_placed;
        
        // Spray state
        bool m_spraying;
        float m_spray_timer;
        float m_spray_duration;  // Time to complete one tag
        
        // Tagged surfaces tracking
        std::vector<uint32> m_tagged_surfaces;
        
        // Current spray target
        uint32 m_target_surface_id;
        Mth::Vector m_spray_position;
        
        void UpdateSpray(float delta_time);
        void CompleteTag();
        bool FindSpraySurface();
    };
}
```

#### 2. Implementation

**New File**: `Code/Sk/Components/SkaterGraffitiComponent.cpp`

```cpp
#include <sk/components/skatergraffiticomponent.h>
#include <gel/collision/collision.h>
#include <sk/objects/skater.h>

namespace Obj
{

DefineSkaComponentClass(CSkaterGraffitiComponent);

CSkaterGraffitiComponent::CSkaterGraffitiComponent()
    : m_tags_remaining(10)
    , m_max_tags(10)
    , m_total_tags_placed(0)
    , m_spraying(false)
    , m_spray_timer(0.0f)
    , m_spray_duration(2.0f)
    , m_target_surface_id(0)
{
}

bool CSkaterGraffitiComponent::CanSprayTag() const
{
    if (m_tags_remaining <= 0) return false;
    if (m_spraying) return false;
    
    // Must be near a sprayable surface
    return FindSpraySurface();
}

bool CSkaterGraffitiComponent::SprayTag()
{
    if (!CanSprayTag()) return false;
    
    // Start spray animation
    CSkater* pSkater = static_cast<CSkater*>(GetObj());
    pSkater->PlaySprayAnimation();
    
    // Begin spray process
    m_spraying = true;
    m_spray_timer = m_spray_duration;
    
    // Find target surface
    FindSpraySurface();
    
    return true;
}

void CSkaterGraffitiComponent::Update()
{
    if (!m_spraying) return;
    
    float delta_time = Tmr::FrameLength();
    m_spray_timer -= delta_time;
    
    // Update spray particle effect
    UpdateSprayEffect(m_spray_position);
    
    if (m_spray_timer <= 0.0f)
    {
        CompleteTag();
    }
}

void CSkaterGraffitiComponent::CompleteTag()
{
    m_spraying = false;
    m_tags_remaining--;
    m_total_tags_placed++;
    
    // Add tag to surface
    PlaceGraffitiDecal(m_target_surface_id, m_spray_position);
    
    // Track tagged surface
    m_tagged_surfaces.push_back(m_target_surface_id);
    
    // Award points
    CSkater* pSkater = static_cast<CSkater*>(GetObj());
    pSkater->AwardPoints(500);  // 500 points per tag
    
    // Play completion sound
    PlayTagCompleteSound();
    
    // Check for goal completion
    CheckGraffitiGoals();
}

bool CSkaterGraffitiComponent::FindSpraySurface()
{
    CSkater* pSkater = static_cast<CSkater*>(GetObj());
    Mth::Vector pos = pSkater->GetPos();
    Mth::Vector facing = pSkater->GetFacing();
    
    // Raycast to find sprayable surface
    Nx::CCollStatic col;
    if (Nx::CCollStatic::sRaycast(pos, facing * 100.0f, col))
    {
        // Check if surface is sprayable
        if (col.flags & mSD_SPRAYABLE)
        {
            m_target_surface_id = col.surface_id;
            m_spray_position = col.point;
            return true;
        }
    }
    
    return false;
}

} // namespace Obj
```

#### 3. Surface Marking

**File**: `Code/Gel/Collision/collision.h` (extend)

```cpp
// Add spray flag to collision flags
enum CollisionFlags
{
    // Existing flags...
    mSD_SPRAYABLE = (1 << 20),  // Surface can be tagged
};
```

#### 4. Level Data Extension

Levels need to mark which surfaces can be tagged:

```cpp
// In level script (QB)
SetSurfaceProperties {
    surface_id = wall_1
    flags = SPRAYABLE
}
```

#### 5. Tag Rendering

**New File**: `Code/Gfx/GraffitiDecal.h`

```cpp
namespace Gfx
{
    class CGraffitiDecal
    {
    public:
        CGraffitiDecal(uint32 texture_id, const Mth::Vector& pos, 
                       const Mth::Vector& normal);
        
        void Render();
        
    private:
        uint32 m_texture_id;
        Mth::Vector m_position;
        Mth::Vector m_normal;
        float m_size;
    };
    
    // Global graffiti manager
    class CGraffitiManager
    {
    public:
        static CGraffitiManager* Instance();
        
        void AddDecal(uint32 surface_id, const Mth::Vector& pos, 
                      const Mth::Vector& normal);
        void RenderAll();
        void Clear();
        
    private:
        std::vector<CGraffitiDecal*> m_decals;
    };
}
```

### Script Integration

```cpp
// New QB commands
bool ScriptSprayTag(Script::CStruct* pParams, Script::CScript* pScript)
{
    Obj::CSkater* pSkater = GetScriptedSkater();
    if (pSkater)
    {
        Obj::CSkaterGraffitiComponent* pGraffiti = 
            pSkater->GetGraffitiComponent();
        if (pGraffiti)
        {
            return pGraffiti->SprayTag();
        }
    }
    return false;
}

bool ScriptGetTagsRemaining(Script::CStruct* pParams, Script::CScript* pScript)
{
    Obj::CSkater* pSkater = GetScriptedSkater();
    if (pSkater)
    {
        Obj::CSkaterGraffitiComponent* pGraffiti = 
            pSkater->GetGraffitiComponent();
        if (pGraffiti)
        {
            int tags = pGraffiti->GetRemainingTags();
            pParams->AddInteger("tags", tags);
            return true;
        }
    }
    return false;
}
```

QB Script usage:
```
SprayTag

GetTagsRemaining
printf "Tags remaining: %i" i = <tags>
```

---

## World Destruction System

### Overview

World Destruction allows players to break certain objects for points and objectives.

### Destructible Object System

#### 1. Base Destructible Class

**New File**: `Code/Sk/Objects/DestructibleObject.h`

```cpp
namespace Obj
{
    class CDestructibleObject : public CMovingObject
    {
    public:
        CDestructibleObject();
        virtual ~CDestructibleObject();
        
        // Destruction interface
        virtual void TakeDamage(float amount, const Mth::Vector& impact_pos);
        virtual void Break();
        virtual bool IsDestroyed() const { return m_destroyed; }
        virtual void Reset();
        
        // Configuration
        void SetHealth(float health) { m_health = m_max_health = health; }
        void SetBreakEffect(uint32 effect_id) { m_break_effect = effect_id; }
        void SetPoints(int points) { m_break_points = points; }
        
    protected:
        float m_health;
        float m_max_health;
        bool m_destroyed;
        
        uint32 m_break_effect;      // Particle effect on break
        uint32 m_break_sound;       // Sound on break
        int m_break_points;         // Points awarded
        
        // Debris system
        std::vector<uint32> m_debris_pieces;
        
        void SpawnDebris();
        void PlayBreakEffect();
        void AwardBreakPoints();
    };
}
```

#### 2. Implementation

**New File**: `Code/Sk/Objects/DestructibleObject.cpp`

```cpp
#include <sk/objects/destructibleobject.h>
#include <gel/scripting/script.h>
#include <sk/modules/skate/score.h>

namespace Obj
{

CDestructibleObject::CDestructibleObject()
    : m_health(100.0f)
    , m_max_health(100.0f)
    , m_destroyed(false)
    , m_break_effect(0)
    , m_break_sound(0)
    , m_break_points(100)
{
}

void CDestructibleObject::TakeDamage(float amount, const Mth::Vector& impact_pos)
{
    if (m_destroyed) return;
    
    m_health -= amount;
    
    // Visual feedback for damage
    PlayHitEffect(impact_pos);
    PlayHitSound();
    
    if (m_health <= 0.0f)
    {
        Break();
    }
}

void CDestructibleObject::Break()
{
    if (m_destroyed) return;
    
    m_destroyed = true;
    
    // Visual effects
    PlayBreakEffect();
    SpawnDebris();
    
    // Audio
    Sfx::CSfxManager::sInstance()->PlaySound(m_break_sound, GetPos());
    
    // Hide original object
    SetVisibility(false);
    SetActive(false);
    
    // Award points
    AwardBreakPoints();
    
    // Notify goals
    NotifyDestructionGoals();
}

void CDestructibleObject::SpawnDebris()
{
    for (auto piece_id : m_debris_pieces)
    {
        // Create debris physics object
        CDebrisPiece* pDebris = new CDebrisPiece(piece_id);
        
        // Set initial velocity (random explosion)
        Mth::Vector velocity = GetRandomExplosionVelocity();
        pDebris->SetVel(velocity);
        
        // Set position near original object
        pDebris->SetPos(GetPos() + GetRandomOffset());
        
        // Add to world
        Mdl::Skate::Instance()->AddDebris(pDebris);
    }
}

void CDestructibleObject::AwardBreakPoints()
{
    // Find nearest skater
    CSkater* pSkater = FindNearestSkater();
    if (pSkater)
    {
        // Award destruction points
        Mdl::Score* pScore = Mdl::Score::Instance();
        if (pScore)
        {
            pScore->AddPoints(m_break_points, pSkater);
        }
    }
}

void CDestructibleObject::Reset()
{
    m_health = m_max_health;
    m_destroyed = false;
    SetVisibility(true);
    SetActive(true);
    
    // Remove any debris
    Mdl::Skate::Instance()->ClearDebris();
}

} // namespace Obj
```

#### 3. Collision Integration

**File**: `Code/Sk/Objects/skater.cpp` (extend)

```cpp
void CSkater::HandleCollision(CObject* pObject)
{
    // Existing collision handling...
    
    // THUG2: Check for destructible objects
    if (pObject->GetType() == OBJTYPE_DESTRUCTIBLE)
    {
        CDestructibleObject* pDestructible = 
            static_cast<CDestructibleObject*>(pObject);
            
        if (!pDestructible->IsDestroyed())
        {
            // Calculate damage based on velocity
            float vel = GetVel().Length();
            float damage = CalculateImpactDamage(vel);
            
            if (damage > 10.0f)  // Minimum damage threshold
            {
                pDestructible->TakeDamage(damage, GetPos());
            }
        }
    }
}

float CSkater::CalculateImpactDamage(float velocity)
{
    // Damage increases with velocity
    float damage = 0.0f;
    
    if (velocity > 500.0f)  // Fast enough to damage
    {
        damage = (velocity - 500.0f) * 0.2f;
    }
    
    // Bonus damage during Freak Out
    if (IsInFreakOutMode())
    {
        damage *= 2.0f;
    }
    
    return damage;
}
```

#### 4. Level Scripting

Define destructible objects in levels:

```
// QB Script for level
CreateDestructibleObject {
    name = mailbox_01
    model = "models/mailbox.mdl"
    pos = (100, 0, 200)
    health = 50
    break_effect = FX_MAILBOX_BREAK
    points = 150
    debris = [ mailbox_piece_1, mailbox_piece_2, mailbox_piece_3 ]
}
```

### Destruction Goals

**File**: `Code/Sk/Modules/Skate/DestructionGoal.h` (new)

```cpp
namespace Game
{
    class CDestructionGoal : public CGoal
    {
    public:
        CDestructionGoal(Script::CStruct* pParams);
        virtual ~CDestructionGoal();
        
        virtual bool Update();
        
        void OnObjectDestroyed(uint32 object_id);
        
    private:
        int m_target_count;
        int m_current_count;
        std::vector<uint32> m_target_objects;
    };
}
```

---

## Sticker Slap Mechanics

### Overview

Sticker slap is a new trick type where the skater slaps stickers on walls while skating past.

### Implementation

#### 1. Trick System Extension

**File**: `Code/Sk/Objects/SkaterTricks.h` (extend)

```cpp
// Add new trick type
enum ETrickType
{
    TRICKTYPE_FLIP,
    TRICKTYPE_GRAB,
    TRICKTYPE_GRIND,
    TRICKTYPE_LIP,
    TRICKTYPE_MANUAL,
    TRICKTYPE_SPECIAL,
    TRICKTYPE_STICKERSLAP,  // THUG2: New trick type
};
```

#### 2. Sticker Slap Component

**New File**: `Code/Sk/Components/SkaterStickerComponent.h`

```cpp
namespace Obj
{
    class CSkaterStickerComponent : public CBaseComponent
    {
    public:
        virtual void Update();
        
        bool CanDoStickerSlap() const;
        bool DoStickerSlap();
        
    private:
        bool m_slapping;
        float m_slap_timer;
        Mth::Vector m_slap_position;
    };
}
```

#### 3. Input Handling

**File**: `Code/Sk/Objects/SkaterPad.cpp` (extend)

```cpp
void CSkaterPad::Update()
{
    // Existing input handling...
    
    // THUG2: Check for sticker slap input
    // (e.g., grind button + direction toward wall)
    if (m_buttons.pressed & BUTTON_GRIND)
    {
        if (IsNearWall())
        {
            CSkaterStickerComponent* pSticker = 
                pSkater->GetStickerComponent();
            if (pSticker && pSticker->CanDoStickerSlap())
            {
                pSticker->DoStickerSlap();
            }
        }
    }
}
```

#### 4. Sticker Rendering

Similar to graffiti system, but instant placement:

```cpp
void CSkaterStickerComponent::DoStickerSlap()
{
    // Find wall
    Mth::Vector wall_pos, wall_normal;
    if (FindNearestWall(wall_pos, wall_normal))
    {
        // Place sticker immediately
        PlaceStickerDecal(wall_pos, wall_normal);
        
        // Play animation
        pSkater->PlaySlapAnimation();
        
        // Award points
        pSkater->AwardPoints(100);
        
        // Add to combo
        AddToCombo("Sticker Slap", 100);
    }
}
```

---

## World Destruction Tour Mode

### Overview

THUG2's story mode is structured as a tour rather than THUG's linear progression.

### Tour System Architecture

#### 1. Tour Manager

**New File**: `Code/Sk/Modules/Skate/TourManager.h`

```cpp
namespace Game
{
    enum ETeam
    {
        TEAM_HAWK,
        TEAM_BAM,
        TEAM_NONE
    };
    
    struct STourLevel
    {
        uint32 level_id;
        const char* city_name;
        bool unlocked;
        int team_hawk_score;
        int team_bam_score;
    };
    
    class CTourManager
    {
    public:
        static CTourManager* Instance();
        
        // Tour progression
        void StartTour();
        void UnlockLevel(uint32 level_id);
        bool IsLevelUnlocked(uint32 level_id) const;
        
        // Team management
        ETeam GetPlayerTeam() const { return m_player_team; }
        void SetPlayerTeam(ETeam team) { m_player_team = team; }
        
        // Scoring
        void AddTeamPoints(ETeam team, int points);
        int GetTeamScore(ETeam team) const;
        
        // Current status
        uint32 GetCurrentLevel() const { return m_current_level; }
        void SetCurrentLevel(uint32 level_id);
        
    private:
        CTourManager();
        
        ETeam m_player_team;
        uint32 m_current_level;
        std::vector<STourLevel> m_levels;
        
        int m_team_hawk_total_score;
        int m_team_bam_total_score;
    };
}
```

#### 2. Team-Based Objectives

Extend goal system for team objectives:

```cpp
class CTeamGoal : public CGoal
{
public:
    CTeamGoal(Script::CStruct* pParams);
    
    virtual bool Win();
    
    void SetTeam(ETeam team) { m_team = team; }
    ETeam GetTeam() const { return m_team; }
    
private:
    ETeam m_team;
    int m_team_points;
};
```

#### 3. Tour Progression Script

```cpp
// QB Script for tour progression

StartWorldTour
{
    SetPlayerTeam team = TEAM_HAWK
    UnlockLevel level = boston
    ShowTourIntro
}

script TourGoalComplete
    if GotParam team_goal
        AddTeamPoints team = <team> points = 1000
    endif
    
    if AllGoalsComplete level = <current_level>
        ShowLevelCompleteMovie
        UnlockNextLevel
    endif
endscript
```

---

## Script Extensions

### New QB Commands for THUG2

```cpp
// Register in Code/Gel/Scripting/scriptcommands.cpp

// Freak Out
REGISTER_COMMAND(ActivateFreakOut)
REGISTER_COMMAND(IsFreakOutActive)
REGISTER_COMMAND(GetFreakOutTimeRemaining)

// Graffiti
REGISTER_COMMAND(SprayTag)
REGISTER_COMMAND(CanSprayTag)
REGISTER_COMMAND(GetTagsRemaining)
REGISTER_COMMAND(AddTags)

// Destruction
REGISTER_COMMAND(BreakObject)
REGISTER_COMMAND(IsObjectDestroyed)
REGISTER_COMMAND(GetDestructionCount)
REGISTER_COMMAND(ResetDestructibleObjects)

// Sticker Slap
REGISTER_COMMAND(DoStickerSlap)
REGISTER_COMMAND(CanStickerSlap)

// Tour
REGISTER_COMMAND(StartWorldTour)
REGISTER_COMMAND(SetPlayerTeam)
REGISTER_COMMAND(GetPlayerTeam)
REGISTER_COMMAND(UnlockTourLevel)
REGISTER_COMMAND(GetTeamScore)
REGISTER_COMMAND(AddTeamPoints)
```

---

## Network Protocol Changes

### Multiplayer Extensions

THUG2 adds new data to sync over network:

```cpp
// Code/Sk/GameNet/ExportMsg.h (extend)

// New message types
enum
{
    // Existing THUG messages...
    
    // THUG2 additions
    MSG_ID_FREAK_OUT_ACTIVATED,
    MSG_ID_FREAK_OUT_DEACTIVATED,
    MSG_ID_TAG_PLACED,
    MSG_ID_OBJECT_DESTROYED,
    MSG_ID_STICKER_PLACED,
    MSG_ID_TEAM_SCORE_UPDATE,
};

// New message structures
struct MsgFreakOutActivated
{
    uint32 skater_id;
    float duration;
};

struct MsgTagPlaced
{
    uint32 skater_id;
    uint32 surface_id;
    float pos_x, pos_y, pos_z;
    uint32 tag_texture;
};

struct MsgObjectDestroyed
{
    uint32 object_id;
    uint32 destroyer_id;
};
```

---

## Asset Pipeline Modifications

### New Asset Types

#### 1. Graffiti Textures
- Location: `Data/Textures/Graffiti/`
- Format: .tex files (THUG texture format)
- Resolution: 256x256 or 512x512
- Naming: `tag_01.tex`, `tag_02.tex`, etc.

#### 2. Sticker Textures
- Location: `Data/Textures/Stickers/`
- Format: .tex files
- Resolution: 128x128
- Transparent backgrounds

#### 3. Destruction Effects
- Location: `Data/Effects/Destruction/`
- Particle definitions
- Break effects
- Debris models

#### 4. Freak Out Effects
- Location: `Data/Effects/FreakOut/`
- Screen effects
- Skater glow/aura effects
- UI animations

### Level Data Extensions

Extend level format to include:

```cpp
struct SLevelData
{
    // Existing THUG data...
    
    // THUG2 additions
    SSprayableSurface* sprayable_surfaces;
    int num_sprayable_surfaces;
    
    SDestructibleObject* destructible_objects;
    int num_destructible_objects;
    
    uint32 tour_level_id;
    ETeam recommended_team;
};
```

---

## Performance Optimization

### Memory Considerations

THUG2 adds several systems that use memory:

```cpp
// Memory budget estimates (PS2: 32MB total)

// THUG base game: ~28MB
// THUG2 additions:
// - Graffiti decals: ~512KB (256 decals @ 2KB each)
// - Destructible objects: ~256KB (64 objects @ 4KB each)
// - Debris pieces: ~512KB (temporary)
// - Sticker decals: ~256KB (512 stickers @ 512B each)
// Total addition: ~1.5MB
```

### Optimization Strategies

#### 1. Graffiti Decal Pooling

```cpp
class CGraffitiDecalPool
{
public:
    static const int MAX_DECALS = 256;
    
    CGraffitiDecal* Allocate();
    void Free(CGraffitiDecal* pDecal);
    
private:
    CGraffitiDecal m_pool[MAX_DECALS];
    std::queue<int> m_free_list;
};
```

#### 2. Debris Lifetime Management

```cpp
void CDebrisPiece::Update(float dt)
{
    m_lifetime -= dt;
    
    // Auto-remove after 10 seconds
    if (m_lifetime <= 0.0f)
    {
        MarkForDeletion();
    }
    
    // Remove if too far from camera
    if (DistanceFromCamera() > 5000.0f)
    {
        MarkForDeletion();
    }
}
```

#### 3. LOD for Destroyed Objects

```cpp
void CDestructibleObject::UpdateLOD()
{
    float dist = DistanceFromCamera();
    
    if (m_destroyed)
    {
        // Show debris only when close
        if (dist < 2000.0f)
        {
            ShowDebris();
        }
        else
        {
            HideDebris();
        }
    }
}
```

---

## Testing and Validation

### Unit Tests

```cpp
// Test Freak Out system
void TestFreakOut()
{
    CSkater* pSkater = CreateTestSkater();
    
    // Test activation
    pSkater->SetSpecial(1.0f);
    pSkater->ActivateFreakOut();
    assert(pSkater->IsInFreakOutMode());
    
    // Test timer
    pSkater->UpdateFreakOut(5.0f);
    assert(pSkater->IsInFreakOutMode());
    
    // Test expiration
    pSkater->UpdateFreakOut(20.0f);
    assert(!pSkater->IsInFreakOutMode());
}
```

### Integration Tests

```cpp
// Test graffiti + goals
void TestGraffitiGoal()
{
    // Create goal requiring 5 tags
    CGraffitiGoal* pGoal = CreateGraffitiGoal(5);
    
    // Place tags
    for (int i = 0; i < 5; i++)
    {
        pGraffiti->SprayTag();
    }
    
    // Verify goal completion
    assert(pGoal->IsComplete());
}
```

---

## Conclusion

This implementation guide provides the technical foundation for adding THUG2 features to the THUG codebase. The modular, component-based architecture of THUG makes it well-suited for these extensions.

### Key Implementation Points

1. **Use Components**: Add new functionality as components
2. **Extend, Don't Replace**: Build on existing systems
3. **Script Support**: Always add QB script commands
4. **Network Sync**: Remember to sync new data in multiplayer
5. **Memory Budget**: Monitor memory usage on target platforms
6. **Testing**: Test thoroughly on all platforms

### Next Steps

1. Implement Freak Out system (simplest addition)
2. Add Tag Graffiti system
3. Implement World Destruction
4. Add Sticker Slap
5. Implement World Destruction Tour
6. Polish and optimize

---

*This document is part of the THUG source code preservation project.*
