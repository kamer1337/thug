# Asset Extraction Guide

This guide explains how to extract assets from the original Tony Hawk's Underground game files for use with this PC port.

## ⚠️ Legal Notice

**Before extracting assets, please read:**

1. You must own a legal copy of Tony Hawk's Underground
2. Assets are copyrighted by Activision and Neversoft
3. Extracted assets are for personal use only
4. Do not distribute extracted assets
5. This guide is for educational and preservation purposes

See `LEGAL.md` for full legal information.

---

## Table of Contents

1. [Requirements](#requirements)
2. [Locating Game Files](#locating-game-files)
3. [Understanding File Structure](#understanding-file-structure)
4. [Two Usage Methods](#two-usage-methods)
5. [Extraction Methods](#extraction-methods)
6. [Asset Conversion](#asset-conversion)
7. [Organizing Extracted Assets](#organizing-extracted-assets)

---

## Requirements

### Software
- Python 3.7+ (for extraction scripts)
- QuickBMS (for archive extraction)
- ImageMagick or similar (for texture conversion)
- Original THUG PC installation

### Tools Location
All extraction tools should be placed in the `tools/` directory:
```
tools/
├── extract_assets.py
├── convert_textures.py
├── convert_models.py
└── quickbms/
```

---

## Locating Game Files

### Default Installation Paths

**Windows:**
```
C:\Program Files (x86)\Activision\Tony Hawk's Underground\
└── Data\
    ├── *.PRE files (archived assets)
    ├── *.WAD files (world data)
    └── *.SKA files (animations)
```

**Steam Version:**
```
C:\Program Files (x86)\Steam\steamapps\common\Tony Hawk's Underground\
```

### File Types in Original Game

| Extension | Content | Location |
|-----------|---------|----------|
| `.PRE` | Compressed archives | Root Data/ |
| `.WAD` | World data | Data/ |
| `.SKA` | Animations | Data/ |
| `.TEX` | Textures | Inside .PRE files |
| `.MDL` | Models | Inside .PRE files |
| `.SCN` | Scenes | Inside .PRE files |
| `.QB` | Scripts | Inside .PRE files |
| `.WAV` | Audio | Data/Sounds/ |

---

## Understanding File Structure

### PRE Files (Pre-archived)

PRE files are the main archive format used by THUG. Structure:

```
PRE File Header
├── Magic: "PPRX" (4 bytes)
├── Version: uint32
├── File Count: uint32
└── TOC Offset: uint32

Table of Contents (TOC)
└── File Entries (per file)
    ├── Name: char[64]
    ├── Offset: uint32
    ├── Size: uint32
    ├── Checksum: uint32
    └── Compression: uint8

Compressed Data
└── Individual files (various formats)
```

### Common PRE Files

- `levels.pre` - All level geometry and collision
- `models.pre` - Character and object models
- `textures.pre` - Texture data
- `anims.pre` - Animation data
- `scripts.pre` - QB script files
- `sounds.pre` - Sound effects

---

## Two Usage Methods

You have two options for using game assets with this engine:

### Option A: Use PRE Archives Directly (Recommended for Initial Setup)

**Advantages:**
- Matches original game behavior exactly
- Faster to set up (just copy files)
- Smaller disk footprint (compressed)
- No extraction needed

**Steps:**
1. Copy `.pre` files from your THUG installation to the `pre/` directory
2. The engine will load assets directly from the archives
3. See `pre/README.md` for details

```bash
# Example: Copy PRE files to pre/ directory
cp "/path/to/THUG/Data/"*.pre ./pre/
```

### Option B: Extract Assets to Data/ Directory (Better for Development)

**Advantages:**
- Easier to modify individual assets
- Better for development and debugging
- Can mix custom assets with original ones
- No need to rebuild archives

**Steps:**
1. Extract assets from `.pre` files using methods below
2. Place extracted files in appropriate `Data/` subdirectories
3. Modify engine code to load from loose files instead of archives

**Note:** The engine currently expects PRE files in `pre/` directory by default. To use extracted assets, you'll need to modify the asset loading code in `Code/Sys/File/` or use both methods simultaneously.

---

## Extraction Methods

### Method 1: Using Python Script (Recommended)

```bash
# Navigate to tools directory
cd tools/

# Extract all assets from a PRE file
python extract_assets.py --input "C:/Path/To/THUG/Data/levels.pre" \
                        --output "../Data/" \
                        --format all

# Extract specific asset types
python extract_assets.py --input "C:/Path/To/THUG/Data/models.pre" \
                        --output "../Data/Models/" \
                        --format mdl

# Extract with conversion
python extract_assets.py --input "C:/Path/To/THUG/Data/textures.pre" \
                        --output "../Data/Textures/" \
                        --format tex \
                        --convert
```

### Method 2: Using QuickBMS

QuickBMS is a generic archive extractor that works with many game formats.

```bash
# Download QuickBMS from: https://aluigi.altervista.org/quickbms.htm

# Use with THUG script
quickbms thug.bms "C:/Path/To/THUG/Data/levels.pre" "./extracted/"
```

### Method 3: Manual Extraction (Advanced)

For developers who want to understand the format:

```python
import struct

def extract_pre_file(pre_path, output_dir):
    with open(pre_path, 'rb') as f:
        # Read header
        magic = f.read(4)
        if magic != b'PPRX':
            raise ValueError("Not a valid PRE file")
        
        version = struct.unpack('<I', f.read(4))[0]
        file_count = struct.unpack('<I', f.read(4))[0]
        toc_offset = struct.unpack('<I', f.read(4))[0]
        
        # Read TOC
        f.seek(toc_offset)
        for i in range(file_count):
            name = f.read(64).decode('ascii').rstrip('\0')
            offset = struct.unpack('<I', f.read(4))[0]
            size = struct.unpack('<I', f.read(4))[0]
            checksum = struct.unpack('<I', f.read(4))[0]
            compression = struct.unpack('<B', f.read(1))[0]
            
            # Extract file
            current_pos = f.tell()
            f.seek(offset)
            data = f.read(size)
            
            # Decompress if needed
            if compression:
                data = decompress_data(data)
            
            # Write file
            output_path = os.path.join(output_dir, name)
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            with open(output_path, 'wb') as out:
                out.write(data)
            
            f.seek(current_pos)
```

---

## Asset Conversion

After extraction, some assets need conversion to modern formats or to match the PC structure.

### Texture Conversion

```bash
# Convert all extracted textures
python tools/convert_textures.py \
    --input "./extracted/textures/" \
    --output "./Data/Textures/" \
    --format dxt5

# Options:
# --format: dxt1, dxt3, dxt5, png, tga
# --resize: Scale textures (0.5, 1.0, 2.0)
# --mipmap: Generate mipmaps (true/false)
```

### Model Conversion

```bash
# Convert models to PC format
python tools/convert_models.py \
    --input "./extracted/models/" \
    --output "./Data/Models/" \
    --optimize

# Options:
# --optimize: Optimize mesh (reduce triangles)
# --split: Split by material
# --tangents: Generate tangent space
```

### Animation Conversion

```bash
# Convert animations
python tools/convert_animations.py \
    --input "./extracted/anims/" \
    --output "./Data/Animations/" \
    --compress

# Options:
# --compress: Enable keyframe compression
# --resample: Resample to different FPS
```

---

## Organizing Extracted Assets

After extraction and conversion, organize assets according to the structure in `Data/README.md`:

### Step 1: Copy to Data Directory

```bash
# Copy models
cp -r ./extracted/models/gameobjects/ ./Data/Models/GameObjects/
cp -r ./extracted/models/vehicles/ ./Data/Models/Vehicles/
cp -r ./extracted/models/characters/ ./Data/Models/Characters/

# Copy textures
cp -r ./extracted/textures/ ./Data/Textures/

# Copy scenes
cp -r ./extracted/levels/ ./Data/Scenes/Levels/

# Copy animations
cp -r ./extracted/anims/ ./Data/Animations/

# Copy audio
cp -r ./extracted/sounds/ ./Data/Audio/SoundFX/
cp -r ./extracted/music/ ./Data/Audio/Music/
```

### Step 2: Verify Organization

```bash
# Run verification script
python tools/verify_assets.py --data-dir ./Data/

# Should output:
# ✓ Models: 1234 files
# ✓ Textures: 567 files
# ✓ Scenes: 12 files
# ✓ Animations: 890 files
# ✓ Audio: 234 files
```

### Step 3: Update Path References

Some extracted files may use old path conventions. Update them:

```bash
# Fix path separators (backslash to forward slash)
python tools/fix_paths.py --data-dir ./Data/

# This updates:
# - Model material references
# - Scene object references
# - Script file paths
```

---

## Batch Extraction Script

For convenience, extract all assets at once:

```bash
#!/bin/bash
# extract_all.sh

THUG_DIR="C:/Program Files (x86)/Activision/Tony Hawk's Underground"
OUTPUT_DIR="./Data"

# Extract PRE files
python tools/extract_assets.py --input "$THUG_DIR/Data/levels.pre" --output "$OUTPUT_DIR/Scenes/"
python tools/extract_assets.py --input "$THUG_DIR/Data/models.pre" --output "$OUTPUT_DIR/Models/"
python tools/extract_assets.py --input "$THUG_DIR/Data/textures.pre" --output "$OUTPUT_DIR/Textures/"
python tools/extract_assets.py --input "$THUG_DIR/Data/anims.pre" --output "$OUTPUT_DIR/Animations/"
python tools/extract_assets.py --input "$THUG_DIR/Data/scripts.pre" --output "$OUTPUT_DIR/Scripts/"

# Convert textures to DXT5
python tools/convert_textures.py --input "$OUTPUT_DIR/Textures/" --format dxt5 --mipmap

# Convert models
python tools/convert_models.py --input "$OUTPUT_DIR/Models/" --optimize

# Copy audio files
cp -r "$THUG_DIR/Data/Sounds/"*.wav "$OUTPUT_DIR/Audio/SoundFX/"
cp -r "$THUG_DIR/Data/Music/"*.ogg "$OUTPUT_DIR/Audio/Music/"

# Verify
python tools/verify_assets.py --data-dir "$OUTPUT_DIR"

echo "Extraction complete!"
```

---

## Troubleshooting

### Problem: "Invalid PRE file" error
**Solution**: Ensure you're using the PC version files, not console versions

### Problem: Corrupted textures after extraction
**Solution**: Try different decompression methods or extract without conversion first

### Problem: Missing files in TOC
**Solution**: Some PRE files may use different formats. Try alternative extraction methods

### Problem: Paths not working in game
**Solution**: Run `fix_paths.py` to convert backslashes to forward slashes

### Problem: Models not loading
**Solution**: Ensure textures are in the correct location and referenced correctly

---

## Advanced Topics

### Creating Custom PRE Files

To create new PRE files with custom content:

```python
# tools/create_pre.py
python create_pre.py --input ./custom_assets/ \
                     --output ./Data/custom.pre \
                     --compress
```

### Modifying Existing Assets

To modify and repack:

1. Extract assets
2. Edit with appropriate tools (Blender, GIMP, etc.)
3. Convert back to THUG format
4. Repack into PRE file

### Level Editing

Level geometry can be edited:

1. Extract `.scn` files
2. Convert to OBJ/FBX
3. Edit in 3D software
4. Convert back to `.scn`
5. Update collision data

---

## Tool Development

### Creating Custom Extraction Tools

The `tools/` directory should contain:

```python
# tools/extract_assets.py
import argparse
import struct
import zlib

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', required=True)
    parser.add_argument('--output', required=True)
    parser.add_argument('--format', default='all')
    parser.add_argument('--convert', action='store_true')
    args = parser.parse_args()
    
    # Extraction logic here
    extract_pre_file(args.input, args.output)

if __name__ == '__main__':
    main()
```

---

## See Also

- `docs/ASSET_FORMATS.md` - Detailed file format specifications
- `Data/README.md` - Asset directory structure
- `tools/README.md` - Tool documentation
- `LEGAL.md` - Legal information and disclaimers

---

**Note**: Asset extraction is for personal use only. Always respect copyright and intellectual property rights.
