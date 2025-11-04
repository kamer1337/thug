# THUG Tools

This directory contains standalone tools for working with Tony Hawk's Underground game assets and data.

## Available Tools

### 1. Model Viewer (`model_viewer/`)

A standalone 3D model, skeleton, and animation viewer/editor for THUG character models.

**Features:**
- Interactive 3D viewport with orbit camera
- Model visualization (wireframe and solid modes)
- Skeleton bone hierarchy display
- Animation playback with controls
- Bone property editing
- Real-time preview

**See:** [`model_viewer/README.md`](model_viewer/README.md) for detailed documentation.

**Quick Start:**
```bash
cd model_viewer
pip install -r requirements.txt
python model_viewer.py
```

## Future Tools

The following tools are planned for future development:

### Asset Extraction Tool
- Extract assets from THUG game archives (.pre files)
- Convert proprietary formats to standard formats
- Batch processing capabilities

### Texture Converter
- Convert THUG texture formats to PNG/TGA
- Support for texture atlases
- UV mapping visualization

### Script Decompiler
- Decompile QB script files
- Syntax highlighting
- Interactive debugging

### Level Viewer
- View and explore THUG levels
- Display collision meshes
- Show spawn points and triggers

## Tool Development Guidelines

When creating new tools:

1. **Standalone**: Each tool should be self-contained and runnable independently
2. **Documentation**: Include comprehensive README with usage examples
3. **Dependencies**: Clearly document all dependencies in requirements.txt
4. **Cross-platform**: Ensure tools work on Windows, Linux, and macOS
5. **Modular**: Use modular design for easy maintenance and extension

## Contributing

To contribute a new tool:

1. Create a new directory under `tools/`
2. Include a detailed README.md
3. Add requirements.txt for dependencies
4. Follow the existing code style
5. Add entry to this main tools README

See the main repository's `CONTRIBUTING.md` for more guidelines.

## Requirements

Different tools may have different requirements. Check each tool's directory for specific dependencies.

Common dependencies:
- Python 3.7+
- PyQt5 (for GUI tools)
- numpy (for math operations)

## License

These tools are part of the THUG source code repository and are provided for educational and preservation purposes. See the main repository's `LEGAL.md` for details.
