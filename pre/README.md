# PRE Archive Files Directory

This directory contains `.pre` (Pre-archived) files from Tony Hawk's Underground. These are compressed archive files that contain game assets in their original packaged format.

## Overview

PRE files are the native archive format used by Tony Hawk's Underground to package and compress game assets. The game engine loads these archives into memory and extracts individual assets as needed.

## Directory Purpose

The `pre/` directory is where the game engine expects to find `.pre` archive files. When the engine calls `Pip::LoadPre("filename.pre")` or `PreMgr::LoadPre("filename.pre")`, it automatically looks in this directory (see `Code/Sys/File/pip.cpp` line 207).

## File Structure

```
pre/
├── README.md           # This file
├── qb.pre              # QB script files
├── skaterparts.pre     # Skater model parts
├── levels.pre          # Level geometry and data
├── models.pre          # Character and object models
├── textures.pre        # Texture data
├── anims.pre           # Animation data
├── sounds.pre          # Sound effects
└── ... (other .pre files from original game)
```

## Common PRE Files

Based on the original game structure, common `.pre` files include:

| Filename | Contents | Size (Approx) |
|----------|----------|---------------|
| `qb.pre` | QB script bytecode | ~5-10 MB |
| `skaterparts.pre` | Skater customization parts | ~20-30 MB |
| `levels.pre` | All level geometry and collision | ~100-200 MB |
| `models.pre` | Character and object 3D models | ~50-100 MB |
| `textures.pre` | Compressed texture data | ~100-150 MB |
| `anims.pre` | Skeletal animation data | ~50-80 MB |
| `sounds.pre` | Sound effects and audio | ~80-120 MB |

## How to Obtain PRE Files

### Legal Requirements

⚠️ **Important**: You must own a legal copy of Tony Hawk's Underground to use these files.

1. Purchase or locate your legal copy of THUG (PC version)
2. Navigate to the game's installation directory
3. Copy the `.pre` files from the `Data/` folder to this `pre/` directory

### Default Installation Paths

**Windows (Retail):**
```
C:\Program Files (x86)\Activision\Tony Hawk's Underground\Data\*.pre
```

**Windows (Steam):**
```
C:\Program Files (x86)\Steam\steamapps\common\Tony Hawk's Underground\Data\*.pre
```

**Copy Command Example:**
```bash
# Windows (PowerShell)
Copy-Item "C:\Program Files (x86)\Activision\Tony Hawk's Underground\Data\*.pre" -Destination ".\pre\"

# Linux/macOS (if you have the Windows version)
cp "/path/to/thug/Data/"*.pre ./pre/
```

## PRE File Format

PRE files use a proprietary compression and archiving format. Structure:

```
PRE File Layout:
├── Header
│   ├── Magic: "PPRX" (4 bytes)
│   ├── Version: 0xabcd0003 (4 bytes)
│   ├── Number of files (4 bytes)
│   └── TOC offset (4 bytes)
├── Table of Contents (TOC)
│   └── For each file:
│       ├── Filename (variable, null-terminated)
│       ├── Data size (4 bytes)
│       ├── Compressed size (4 bytes)
│       ├── Checksum (4 bytes)
│       └── Flags (2 bytes)
└── Compressed Data
    └── Individual files (LZ-compressed)
```

## Working with PRE Files

### Loading in Game Engine

The game engine provides two methods to work with PRE files:

**Method 1: PIP (Pre-In-Place)**
```cpp
// Load a PRE file
Pip::LoadPre("qb.pre");

// Load a file from within the PRE
void* data = Pip::Load("scripts/startup.qb");

// Unload when done
Pip::Unload("scripts/startup.qb");
Pip::UnloadPre("qb.pre");
```

**Method 2: PreMgr (Pre Manager)**
```cpp
// Get the PreMgr instance
File::PreMgr* pre_mgr = File::PreMgr::Instance();

// Load a PRE file
pre_mgr->LoadPre("skaterparts.pre", false);  // false = synchronous load

// Load a file from within the PRE
int size;
void* data = pre_mgr->LoadFile("models/skater_head_01.mdl", &size);

// Unload when done
pre_mgr->UnloadPre("skaterparts.pre");
```

### Extracting PRE Files

If you want to extract assets from `.pre` files to the `Data/` directory structure:

1. See `docs/ASSET_EXTRACTION.md` for extraction tools and procedures
2. Use the Python extraction scripts in `tools/` (when available)
3. Or use QuickBMS with appropriate scripts

**Note**: The `Data/` directory is for extracted/loose assets, while `pre/` is for the original archive files.

## Development Workflow

### Option 1: Using PRE Archives (Original Method)
- Place `.pre` files in this directory
- Game loads assets directly from archives
- Faster startup (no individual file operations)
- Matches original game behavior

### Option 2: Using Extracted Assets (Modern Method)
- Extract assets to `Data/` directory structure
- Modify code to load from loose files instead of archives
- Easier to modify and test individual assets
- Better for development and debugging

### Option 3: Hybrid Approach
- Keep some assets in `.pre` archives
- Extract frequently modified assets to `Data/`
- Configure which method to use per asset type

## Version Control

### What to Commit

✅ **DO commit:**
- This README.md
- `.gitkeep` file (to preserve directory structure)
- Documentation about PRE files

❌ **DO NOT commit:**
- Actual `.pre` files (they are copyrighted)
- Extracted assets from `.pre` files

### .gitignore Configuration

The `.gitignore` file is configured to:
```gitignore
# Ignore all .pre files in the pre/ directory
pre/*.pre

# But keep documentation and structure
!pre/README.md
!pre/.gitkeep
```

## Troubleshooting

### "Could not open file pre\filename.pre"

**Cause**: The `.pre` file is missing from this directory.

**Solution**:
1. Check if the file exists: `ls -la pre/filename.pre`
2. Copy it from your THUG installation
3. Verify filename matches exactly (case-sensitive on Linux/macOS)

### "File X failed to load quickly"

**Cause**: File permissions or corruption.

**Solution**:
1. Check file permissions: `chmod 644 pre/*.pre`
2. Verify file integrity (compare size with original)
3. Re-copy from original game installation

### "Reached limit of 100 pre files loaded at once"

**Cause**: Too many PRE files loaded simultaneously.

**Solution**:
1. Unload PRE files when no longer needed
2. Increase `MAX_PRE_FILES` in `Code/Sys/File/pip.cpp` if necessary
3. Consolidate assets into fewer PRE files

## Performance Considerations

### Loading Speed
- PRE files load faster than individual files (reduced I/O operations)
- Larger PRE files take longer to initially load into memory
- Once loaded, asset access is very fast (in-memory)

### Memory Usage
- PRE files are loaded entirely into memory
- Plan ~500 MB - 1 GB RAM for all loaded PRE files
- Unload PRE files not needed for current level

### Best Practices
1. Load only required PRE files for current game state
2. Load large PRE files asynchronously to avoid frame drops
3. Unload PRE files when transitioning between levels
4. Monitor memory usage with `Pip::DumpPipPreStatus()`

## Related Documentation

- `docs/ASSET_EXTRACTION.md` - How to extract assets from PRE files
- `docs/ASSET_FORMATS.md` - Details on file formats inside PRE archives
- `Data/README.md` - Documentation for extracted asset structure
- `Code/Sys/File/pip.cpp` - PIP (Pre-In-Place) implementation
- `Code/Sys/File/PRE.cpp` - PreMgr implementation

## Technical Details

### Compression Algorithm

PRE files use a custom LZ-based compression algorithm:
- Similar to LZSS (Lempel-Ziv-Storer-Szymanski)
- Ring buffer size: 4096 bytes
- Match limit: 18 bytes
- Threshold: 2 bytes

### In-Place Decompression

The PIP system supports in-place decompression:
- Decompresses directly in the loaded memory buffer
- Requires `IN_PLACE_DECOMPRESSION_MARGIN` (3072 bytes) extra space
- Reduces memory allocations and copying

### Quad-Word Alignment

All contained files are aligned to 16-byte boundaries:
- Required by collision detection code
- Improves cache performance
- Necessary for SIMD operations

## See Also

- Original Neversoft documentation (if available)
- Tony Hawk game modding community resources
- QuickBMS format scripts for PRE files

---

**Legal Notice**: Tony Hawk's Underground and its assets are copyrighted by Activision and Neversoft Entertainment. This documentation is for educational and preservation purposes. Only use assets you have legal rights to possess.
