# THUG vs THUG2: Quick Reference Guide

## At a Glance

| Aspect | THUG (2003) | THUG2 (2004) | Code Impact |
|--------|-------------|--------------|-------------|
| **Release** | October 2003 | October 2004 | N/A |
| **Developer** | Neversoft | Neversoft | Same team |
| **Engine** | Neversoft Engine | Neversoft Engine | Same foundation |
| **Platforms** | PS2, Xbox, GC, PC, GBA | PS2, Xbox, GC, PC, GBA, PSP | PC additions |
| **Code Reuse** | N/A | 70-80% from THUG | High compatibility |

## Story Mode Comparison

| Feature | THUG | THUG2 |
|---------|------|-------|
| **Structure** | Linear storyline | World Destruction Tour |
| **Theme** | Rise to pro status | Anarchic world tour |
| **Progression** | Goal-based unlocking | Tour-based progression |
| **Teams** | Solo career | Team Hawk vs Team Bam |
| **Locations** | 9 levels | 13+ levels |

## Gameplay Features

### Core Mechanics (Identical)
✓ Physics-based skating  
✓ Trick system (flip, grab, grind, lip, manual)  
✓ Special meter  
✓ Combo system  
✓ Balance mechanics  
✓ Reverts and spine transfers  

### THUG Features
- Story mode with custom character
- Off-board walking
- Create-A-Trick (CAT)
- Vehicle driving
- Classic mode
- Create-A-Park
- Online multiplayer

### THUG2 Exclusive Features
- **Freak Out mode** - Unlimited specials for limited time
- **Tag Graffiti** - Spray paint tags on surfaces
- **World Destruction** - Break objects for points
- **Sticker Slap** - New trick type
- **Face mapping** - Import photos for characters
- **Enhanced park editor** - More pieces and themes

## Technical Specifications

### Engine Systems

| System | THUG | THUG2 | Changes |
|--------|------|-------|---------|
| **Physics** | ✓ | ✓ | Minor enhancements |
| **Graphics** | Vulcan renderer | Vulcan renderer | Effect improvements |
| **Scripting** | QB language | QB language | New commands |
| **Audio** | Music/SFX system | Music/SFX system | New sounds |
| **Network** | GameSpy | GameSpy | Protocol additions |
| **Save System** | Memory card | Memory card | Extended data |

### Code Architecture

```
Code Reusability by Layer:
┌─────────────────────────────────────┐
│ Core (100% reusable)                │ ← Math, Memory, Strings
├─────────────────────────────────────┤
│ System (95% reusable)               │ ← File I/O, Config
├─────────────────────────────────────┤
│ GEL Engine (90% reusable)           │ ← Scripting, Assets
├─────────────────────────────────────┤
│ Graphics (90% reusable)             │ ← Rendering, Effects
├─────────────────────────────────────┤
│ Physics (90% reusable)              │ ← Collision, Movement
├─────────────────────────────────────┤
│ Game Logic (70% reusable)           │ ← Goals, Multiplayer
├─────────────────────────────────────┤
│ Content (30% reusable)              │ ← Story, Levels, Assets
└─────────────────────────────────────┘
```

## Implementation Priorities

### Phase 1: Core Extensions (Weeks 1-4)
1. ✓ Freak Out system
2. ✓ Basic destruction physics
3. ✓ Sticker slap trick type

### Phase 2: Major Systems (Weeks 5-12)
1. ✓ Tag Graffiti component
2. ✓ Destructible objects
3. ✓ World Destruction Tour mode
4. ✓ Team-based objectives

### Phase 3: Content (Weeks 13-24)
1. New level designs
2. Story mode scripts
3. Enhanced park editor
4. Face mapping system

### Phase 4: Polish (Weeks 25-32)
1. Graphics enhancements
2. Online improvements
3. Optimization
4. Testing

## Key Differences Summary

### What Changed
- **Story structure**: Linear → Tour-based
- **Special meter**: Standard → Freak Out mode available
- **Environment**: Static → Destructible objects
- **Tricks**: Standard → Added sticker slap
- **Scoring**: Standard → Destruction bonuses
- **Progression**: Goal-based → Tour/team-based

### What Stayed the Same
- Core skating physics
- Trick system fundamentals
- Control scheme
- Create-A-Skater
- Create-A-Park (enhanced)
- Classic mode
- Multiplayer modes (enhanced)
- Scripting system (extended)

## File Locations

### THUG2-Specific Code (New)
```
Code/Sk/Components/
    SkaterGraffitiComponent.h/.cpp
    SkaterFreakOutComponent.h/.cpp
    SkaterStickerComponent.h/.cpp
    
Code/Sk/Objects/
    DestructibleObject.h/.cpp
    DebrisPiece.h/.cpp
    
Code/Sk/Modules/Skate/
    TourManager.h/.cpp
    DestructionGoal.h/.cpp
    TeamGoal.h/.cpp
    
Code/Gfx/
    GraffitiDecal.h/.cpp
    DestructionEffect.h/.cpp
```

### Modified THUG Files
```
Code/Sk/Objects/
    skater.h/.cpp           (Freak Out state)
    SkaterTricks.h/.cpp     (Sticker slap)
    
Code/Sk/Modules/Skate/
    GoalManager.h/.cpp      (Tour mode)
    score.h/.cpp            (Destruction points)
    
Code/Gel/Scripting/
    scriptcommands.cpp      (New QB commands)
    
Code/Sk/GameNet/
    ExportMsg.h             (Network messages)
```

## Memory Budget

### Platform Memory
- **PS2**: 32 MB RAM
- **Xbox**: 64 MB RAM  
- **GameCube**: 24 MB RAM
- **PC**: 256+ MB RAM

### THUG2 Memory Additions
- Graffiti decals: ~512 KB
- Destructible objects: ~256 KB
- Debris system: ~512 KB (temporary)
- Sticker decals: ~256 KB
- **Total**: ~1.5 MB additional

## Performance Impact

### THUG Baseline
- 60 FPS target (30 FPS on consoles in some areas)
- Level streaming
- LOD system
- Occlusion culling

### THUG2 Additions
- Destruction physics: ~5% CPU overhead
- Graffiti rendering: ~2% GPU overhead
- Debris simulation: ~5% CPU overhead (temporary)
- **Total**: ~10-15% performance impact (optimized)

## Script Command Reference

### New QB Commands for THUG2

```
Freak Out:
  ActivateFreakOut
  IsFreakOutActive
  GetFreakOutTimeRemaining

Graffiti:
  SprayTag
  CanSprayTag
  GetTagsRemaining
  AddTags

Destruction:
  BreakObject
  IsObjectDestroyed
  GetDestructionCount
  ResetDestructibleObjects

Sticker Slap:
  DoStickerSlap
  CanStickerSlap

Tour Mode:
  StartWorldTour
  SetPlayerTeam
  GetPlayerTeam
  UnlockTourLevel
  GetTeamScore
  AddTeamPoints
```

## Asset Requirements

### New Asset Types for THUG2
1. **Graffiti Textures** - 256x256 or 512x512 .tex files
2. **Sticker Textures** - 128x128 .tex files with alpha
3. **Destruction Effects** - Particle system definitions
4. **Debris Models** - Low-poly broken pieces
5. **Freak Out Effects** - Screen effects and skater glow
6. **New Animations** - Spray, sticker slap, freak out moves

### Asset Compatibility
✓ Model format (.mdl) - Compatible  
✓ Texture format (.tex) - Compatible  
✓ Animation format - Compatible  
✓ Script format (.qb) - Compatible with new commands  
✓ Level format - Extended for destruction data  

## Developer Quick Start

### To Add Freak Out to THUG
1. Add `m_freak_out_active` to `CSkater`
2. Implement `ActivateFreakOut()` / `DeactivateFreakOut()`
3. Modify `CanDoSpecialTrick()` check
4. Add HUD timer display
5. Register QB script commands

### To Add Tag Graffiti to THUG
1. Create `CSkaterGraffitiComponent`
2. Mark surfaces as sprayable in collision
3. Implement raycast to find spray targets
4. Create graffiti decal rendering system
5. Add spray animation and effects
6. Register QB script commands

### To Add Destruction to THUG
1. Create `CDestructibleObject` class
2. Implement damage and break mechanics
3. Create debris physics system
4. Add destruction particle effects
5. Modify collision to check for destructibles
6. Register QB script commands

## Testing Checklist

### Core Systems
- [ ] Freak Out activates with full special meter
- [ ] Freak Out timer counts down properly
- [ ] Special tricks work during Freak Out
- [ ] Tag spray completes after timer
- [ ] Graffiti decals render correctly
- [ ] Destructible objects break on impact
- [ ] Debris spawns and times out properly
- [ ] Sticker slap places stickers instantly

### Multiplayer
- [ ] Freak Out syncs across clients
- [ ] Graffiti tags visible to all players
- [ ] Destruction syncs properly
- [ ] Team scores update correctly

### Performance
- [ ] 30 FPS maintained on PS2
- [ ] Memory budget not exceeded
- [ ] Debris cleans up properly
- [ ] LOD system works for destroyed objects

## Common Pitfalls

### ⚠️ Avoid These Mistakes

1. **Memory Leaks**
   - Always pool graffiti decals
   - Clean up debris after timeout
   - Free destroyed object resources

2. **Network Desyncs**
   - Sync all new state (Freak Out, destruction)
   - Send reliable messages for permanent changes
   - Use checksums for graffiti/destruction state

3. **Performance Issues**
   - Limit max active debris pieces
   - Use LOD for distant destroyed objects
   - Cull graffiti decals outside view

4. **Save/Load**
   - Save graffiti tag positions
   - Save destruction state
   - Save tour progression

## Resources

### Documentation
- [THUG2_RESEARCH.md](THUG2_RESEARCH.md) - Full comparison
- [THUG2_IMPLEMENTATION.md](THUG2_IMPLEMENTATION.md) - Implementation guide
- [subsystems/RECORDS.md](subsystems/RECORDS.md) - Records system

### External Links
- [THUG Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground)
- [THUG2 Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground_2)

---

## Summary

**THUG2 is THUG plus:**
- Freak Out mode
- Tag graffiti
- World destruction  
- Sticker slap
- Tour-based story
- ~20-30% new code
- Same engine foundation

**Development Time:** ~12 months with full team  
**Code Reuse:** 70-80%  
**Key Strategy:** Extend, don't replace

---

*Last Updated: 2024-10*
*Quick Reference Version: 1.0*
