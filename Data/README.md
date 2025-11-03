# THUG PC Assets Directory Structure

This directory contains all game assets for Tony Hawk's Underground PC port. The structure is organized to match the original game's asset organization while being compatible with modern PC file systems.

## Directory Structure

```
Data/
├── Models/              # 3D models and geometry
│   ├── GameObjects/     # Interactive game objects
│   │   ├── Skate/       # SKATE letters (S, K, A, T, E)
│   │   ├── Combo/       # COMBO letters (C, O, M, B, O)
│   │   └── Collectibles/ # Other collectible items
│   ├── Vehicles/        # Vehicles and movable objects
│   ├── Characters/      # Skater and NPC models
│   ├── Props/           # Static and dynamic props
│   └── UI/              # UI 3D elements
├── Textures/            # Texture files
│   ├── Environment/     # Level textures
│   ├── Characters/      # Character textures
│   ├── UI/              # User interface textures
│   └── Effects/         # Particle and effect textures
├── Scenes/              # Level geometry and scene data
│   └── Levels/          # Individual level files
├── Animations/          # Animation data files
├── Skeletons/           # Skeletal rig definitions
├── Skins/               # Skin/mesh binding data
├── Collision/           # Collision geometry
├── Cutscenes/           # Cutscene data
├── Audio/               # Audio files
│   ├── Music/           # Background music
│   ├── SoundFX/         # Sound effects
│   └── Streams/         # Streaming audio
├── Scripts/             # QB script files
└── Config/              # Configuration files
```

## File Formats

### Models (.mdl)
- 3D mesh geometry
- Material references
- LOD levels
- Bounding volumes

Example paths:
- `Data/Models/GameObjects/Skate/letter_a.mdl`
- `Data/Models/Characters/skater_01.mdl`

### Textures (.tex, .img)
- Compressed texture data
- Multiple mipmap levels
- Various formats (DXT1, DXT3, DXT5)

Example paths:
- `Data/Textures/Environment/wall_brick_01.tex`
- `Data/Textures/Characters/skater_body.tex`

### Scenes (.scn)
- Level geometry
- Object placement
- Lighting data
- Trigger volumes

Example paths:
- `Data/Scenes/Levels/NJ.scn`
- `Data/Scenes/Levels/Manhattan.scn`

### Animations (.ska)
- Skeletal animation keyframes
- Animation compression
- Blend weights

Example paths:
- `Data/Animations/Skater/ollie.ska`
- `Data/Animations/Skater/kickflip.ska`

### Collision (.col)
- Collision mesh data
- Surface properties
- Collision flags

Example paths:
- `Data/Collision/NJ.col`
- `Data/Collision/Manhattan.col`

### Audio
- Music: `.wav`, `.ogg`, `.mp3`
- Sound effects: `.wav`
- Streams: `.wav`, `.ogg`

Example paths:
- `Data/Audio/Music/main_theme.ogg`
- `Data/Audio/SoundFX/grind_01.wav`

## Asset Loading

Assets are loaded through the Asset Manager (`Code/Gel/AssMan/`):

```cpp
// Example: Loading a model
Ass::CManager* pAssetMgr = Ass::CManager::Instance();
Ass::CAsset* pAsset = pAssetMgr->Load("Data/Models/GameObjects/Skate/letter_a.mdl", 
                                       Ass::ASSET_SCENE);
```

## Path Conventions

### Windows Compatibility
- Use forward slashes (`/`) in code for cross-platform compatibility
- The engine automatically converts to backslashes on Windows
- Examples in code use backslashes for historical reasons but should be updated

### Case Sensitivity
- Be case-sensitive aware (Linux/Mac are case-sensitive)
- Use lowercase for all directory and file names when possible
- The repository includes symlinks for case-insensitive compatibility

## Asset Naming Conventions

### Models
- Format: `{object_name}.mdl`
- Examples: `letter_a.mdl`, `skateboard_01.mdl`, `car_taxi.mdl`

### Textures
- Format: `{object}_{material}_{variant}.tex`
- Examples: `wall_brick_01.tex`, `character_body_01.tex`

### Animations
- Format: `{action}.ska`
- Examples: `ollie.ska`, `kickflip.ska`, `grind_50_50.ska`

## Adding New Assets

To add a new asset to the game:

1. **Place the file** in the appropriate directory
2. **Follow naming conventions** for consistency
3. **Update level scripts** if needed (QB scripts in `Data/Scripts/`)
4. **Reference in code** using relative paths from `Data/`

Example:
```cpp
// In level script (QB)
CreateObject {
    name = letter_a_01
    model = "Data/Models/GameObjects/Skate/letter_a.mdl"
    pos = (100, 0, 200)
}
```

## Asset Organization Best Practices

1. **Keep related assets together**: Models, textures, and animations for the same object should use similar naming
2. **Use subdirectories**: Don't put all assets in one folder
3. **Version control**: Large binary assets should use Git LFS
4. **Compression**: Use appropriate compression for textures and audio
5. **LOD models**: Include multiple LOD levels for performance

## Level-Specific Assets

Each level should have its own subdirectory:

```
Data/Scenes/Levels/NJ/
├── NJ.scn              # Scene geometry
├── NJ.col              # Collision data
├── NJ_textures.tex     # Level-specific textures
└── NJ_script.qb        # Level script
```

## Performance Considerations

- **Texture sizes**: Keep textures power-of-two (256, 512, 1024, 2048)
- **Model complexity**: Target 1000-5000 triangles for props, 5000-15000 for main objects
- **Audio quality**: Use appropriate bit rates (128-192 kbps for music, 44.1 kHz for effects)
- **File streaming**: Large assets should support streaming

## Platform-Specific Notes

### Windows (Win32)
- Full DirectX support
- Audio via DirectSound or SDL2_mixer
- Model loading via Direct3D

### Linux
- OpenGL rendering
- Audio via SDL2_mixer or OpenAL
- Model conversion may be needed

### macOS
- Metal or OpenGL rendering
- Audio via SDL2_mixer
- Same as Linux for models

## Missing Assets

**Note**: This repository does not include actual game assets due to copyright. You will need:
- Original game files (from THUG PC installation)
- Asset extraction tools (see `tools/` directory)
- Or create your own assets following the formats

## Asset Extraction

See `docs/ASSET_EXTRACTION.md` for information on extracting assets from the original game files.

## See Also

- `docs/ASSET_FORMATS.md` - Detailed file format specifications
- `docs/BUILDING.md` - Build instructions including asset pipeline
- `Code/Gel/AssMan/` - Asset Manager implementation
- `Code/Sys/File/` - File I/O system

---

**Legal Note**: Game assets are copyrighted by Activision and Neversoft. This structure is provided for educational and preservation purposes. Use only assets you have legal rights to.
