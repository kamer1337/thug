# Asset Manifest for THUG PC Port

This file lists all required assets for the game to function. Use this as a checklist when extracting assets from the original game.

## Core Assets (Required)

### Models - GameObjects
```
Data/Models/GameObjects/Skate/
├── letter_s.mdl
├── letter_k.mdl
├── letter_a.mdl
├── letter_t.mdl
└── letter_e.mdl

Data/Models/GameObjects/Combo/
├── goal_combo_c.mdl
├── goal_combo_o.mdl
├── goal_combo_m.mdl
├── goal_combo_b.mdl
└── goal_combo_o.mdl (duplicate)

Data/Models/GameObjects/Collectibles/
├── coin.mdl
├── stat_point.mdl
└── deck.mdl
```

### Models - Characters
```
Data/Models/Characters/
├── skater_01.mdl (default skater)
├── skater_02.mdl (female skater)
├── tony_hawk.mdl
├── bam_margera.mdl
├── ped_01.mdl (generic pedestrian)
├── ped_02.mdl
└── ... (other pro skaters and NPCs)
```

### Models - Vehicles
```
Data/Models/Vehicles/
├── car_taxi.mdl
├── car_sedan.mdl
├── car_sports.mdl
└── skateboard_default.mdl
```

### Models - Props
```
Data/Models/Props/
├── bench.mdl
├── rail_01.mdl
├── rail_02.mdl
├── quarter_pipe.mdl
├── half_pipe.mdl
├── mailbox.mdl
└── ... (various level props)
```

### Textures - Environment
```
Data/Textures/Environment/
├── ground_concrete_01.tex
├── wall_brick_01.tex
├── wall_graffiti_01.tex
├── metal_rusted_01.tex
├── wood_01.tex
└── ... (level textures)
```

### Textures - Characters
```
Data/Textures/Characters/
├── skater_body_01.tex
├── skater_face_01.tex
├── skater_clothes_01.tex
└── ... (character textures)
```

### Textures - UI
```
Data/Textures/UI/
├── menu_background.tex
├── button_default.tex
├── button_hover.tex
├── icon_trick_01.tex
├── hud_special_meter.tex
└── ... (UI textures)
```

### Scenes - Levels
```
Data/Scenes/Levels/
├── NJ/
│   ├── NJ.scn
│   ├── NJ.col
│   └── NJ_textures.tex
├── Manhattan/
│   ├── Manhattan.scn
│   ├── Manhattan.col
│   └── Manhattan_textures.tex
├── Tampa/
├── San Diego/
├── Hawaii/
├── Vancouver/
├── Slam City Jam/
└── Moscow/
```

### Animations - Skater
```
Data/Animations/
├── idle.ska
├── push.ska
├── ollie.ska
├── kickflip.ska
├── heelflip.ska
├── grind_50_50.ska
├── grind_5_0.ska
├── grind_nosegrind.ska
├── manual.ska
├── manual_nose.ska
├── wallride.ska
└── ... (100+ trick animations)
```

### Audio - Music
```
Data/Audio/Music/
├── main_theme.ogg
├── menu_music.ogg
├── level_01.ogg
├── level_02.ogg
└── ... (soundtrack files)
```

### Audio - SoundFX
```
Data/Audio/SoundFX/
├── grind_01.wav
├── land_01.wav
├── ollie_01.wav
├── collision_01.wav
├── special_activated.wav
└── ... (sound effects)
```

### Collision Data
```
Data/Collision/
├── NJ.col
├── Manhattan.col
├── Tampa.col
└── ... (per level)
```

### Scripts
```
Data/Scripts/
├── main.qb
├── levels/
│   ├── NJ.qb
│   ├── Manhattan.qb
│   └── ...
├── goals/
│   ├── story_goals.qb
│   └── classic_goals.qb
└── ui/
    ├── menu.qb
    └── hud.qb
```

---

## Asset Counts

| Category | Count | Status |
|----------|-------|--------|
| Models (GameObjects) | 50+ | ⚠️ Extract |
| Models (Characters) | 30+ | ⚠️ Extract |
| Models (Props) | 200+ | ⚠️ Extract |
| Textures | 1000+ | ⚠️ Extract |
| Scenes | 12 | ⚠️ Extract |
| Animations | 150+ | ⚠️ Extract |
| Audio (Music) | 20+ | ⚠️ Extract |
| Audio (SFX) | 200+ | ⚠️ Extract |
| Scripts | 100+ | ⚠️ Extract |

**Legend:**
- ✅ Included
- ⚠️ Needs extraction from original game
- ❌ Missing

---

## Optional Assets

These assets enhance the experience but are not required for basic functionality:

### High-Resolution Textures
```
Data/Textures/HiRes/
└── (upscaled textures for modern displays)
```

### Additional Levels
```
Data/Scenes/Levels/Custom/
└── (community-created levels)
```

### Custom Skaters
```
Data/Models/Characters/Custom/
└── (custom character models)
```

---

## Extraction Priority

### Phase 1: Core Gameplay (High Priority)
1. ✅ Directory structure
2. ⚠️ Main menu textures and UI
3. ⚠️ Default skater model and textures
4. ⚠️ Basic animations (idle, push, ollie, kickflip)
5. ⚠️ One test level (New Jersey recommended)
6. ⚠️ Basic sound effects

### Phase 2: Full Gameplay (Medium Priority)
1. ⚠️ All level geometry
2. ⚠️ All skater animations
3. ⚠️ All character models
4. ⚠️ Complete sound effects
5. ⚠️ Background music
6. ⚠️ All scripts

### Phase 3: Polish (Low Priority)
1. ⚠️ High-resolution textures
2. ⚠️ All pro skater models
3. ⚠️ Bonus levels
4. ⚠️ Cutscene data
5. ⚠️ Extra content

---

## Verification

After extracting assets, verify completeness:

```bash
# Run verification script
python tools/verify_assets.py --manifest Data/ASSET_MANIFEST.md

# Expected output:
# Checking Models...
#   GameObjects: 45/50 found (90%)
#   Characters: 28/30 found (93%)
# Checking Textures...
#   Environment: 856/1000 found (85%)
# ... etc
```

---

## Size Estimates

| Category | Approximate Size |
|----------|-----------------|
| Models | 500 MB |
| Textures | 1.5 GB |
| Scenes | 200 MB |
| Animations | 300 MB |
| Audio | 1 GB |
| Scripts | 50 MB |
| **Total** | **~3.5 GB** |

---

## Notes

- File sizes are approximate and depend on compression
- Original PC installation requires ~3.2 GB
- Extracted uncompressed assets may be larger
- Some assets may be duplicated across levels
- Not all original assets may be needed (console-specific content removed)

---

## See Also

- `docs/ASSET_EXTRACTION.md` - How to extract assets
- `docs/ASSET_FORMATS.md` - Asset file format details
- `Data/README.md` - Directory structure
- `tools/verify_assets.py` - Asset verification script

---

**Status**: ⚠️ Assets must be extracted from original game
**Last Updated**: 2024
