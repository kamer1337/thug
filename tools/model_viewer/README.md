# THUG 3D Model Viewer & Skeleton/Animation Editor

A standalone tool for viewing, editing, and analyzing 3D character models, skeletons, and animations from Tony Hawk's Underground.

## Features

### Model Viewer
- Load and display 3D character models (.mdl files)
- Interactive 3D viewport with orbit camera controls
- Wireframe and solid rendering modes
- Model statistics display (vertex count, polygon count, etc.)

### Skeleton Viewer
- Display skeleton/bone hierarchy
- Visualize bone connections and joints
- Show bone names and IDs
- Navigate through bone tree structure
- Highlight individual bones

### Animation Editor/Viewer
- Load animation files (.ska files)
- Playback controls (play, pause, stop, step forward/backward)
- Animation timeline with frame scrubbing
- Speed control (0.1x to 4.0x)
- Loop modes (once, loop, ping-pong)
- Frame-by-frame stepping
- Display current frame and total frames
- Animation information panel

### Editor Features
- Select and manipulate individual bones
- View and edit bone transformations (position, rotation, scale)
- Real-time preview of changes
- Export modified skeletons and animations
- Undo/redo support

## Requirements

### Python Dependencies
```bash
pip install PyQt5 PyOpenGL numpy
```

### Optional Dependencies
For enhanced features:
```bash
pip install pillow  # For texture support
```

## Usage

### Basic Viewer Mode
```bash
python model_viewer.py
```

### Load a model directly
```bash
python model_viewer.py --model path/to/character.mdl
```

### Load model with animations
```bash
python model_viewer.py --model path/to/character.mdl --anim path/to/animation.ska
```

### Command Line Options
```
--model PATH       Load model file
--skeleton PATH    Load skeleton file
--anim PATH        Load animation file
--fullscreen       Start in fullscreen mode
--wireframe        Start in wireframe mode
```

## Controls

### Camera Controls
- **Left Mouse + Drag**: Orbit camera around model
- **Right Mouse + Drag**: Pan camera
- **Mouse Wheel**: Zoom in/out
- **Middle Mouse**: Reset camera

### Animation Controls
- **Space**: Play/Pause animation
- **Left Arrow**: Previous frame
- **Right Arrow**: Next frame
- **Home**: Go to first frame
- **End**: Go to last frame
- **+/-**: Increase/decrease playback speed
- **R**: Reset animation to first frame

### View Controls
- **W**: Toggle wireframe mode
- **S**: Toggle skeleton overlay
- **B**: Toggle bone names
- **G**: Toggle grid
- **F**: Focus on selected bone
- **H**: Show/hide UI

## File Formats

### Supported Model Formats
- `.mdl` - THUG model format (primary)
- `.ska` - THUG skeleton/animation format
- `.tex` - THUG texture format

See `docs/ASSET_FORMATS.md` in the main repository for detailed format specifications.

## Architecture

The tool is built with a modular architecture:

```
tools/model_viewer/
├── model_viewer.py       # Main application entry point
├── ui/
│   ├── main_window.py    # Main window and UI layout
│   ├── viewport.py       # 3D OpenGL viewport widget
│   ├── animation_panel.py # Animation controls panel
│   ├── skeleton_tree.py  # Skeleton hierarchy tree view
│   └── properties_panel.py # Bone properties editor
├── core/
│   ├── model_loader.py   # Model file loading and parsing
│   ├── skeleton.py       # Skeleton data structures
│   ├── animation.py      # Animation data and playback
│   ├── renderer.py       # OpenGL rendering
│   └── camera.py         # Camera controls
└── utils/
    ├── file_formats.py   # THUG file format parsers
    └── math_utils.py     # Math utilities for transformations
```

## Development

### Adding New Features
1. Model loading is handled in `core/model_loader.py`
2. Rendering logic is in `core/renderer.py`
3. UI components are in the `ui/` directory
4. File format parsers are in `utils/file_formats.py`

### Testing
```bash
# Run with test model (if available)
python model_viewer.py --model ../Data/Models/Characters/test_character.mdl
```

## Known Limitations

- Currently supports THUG file formats only
- Texture support is basic (requires proper texture extraction)
- Animation editing is limited to viewing and basic modifications
- Export functionality requires the original game files for reference

## Future Enhancements

- [ ] Full texture support with UV mapping
- [ ] Advanced animation editing (keyframe manipulation)
- [ ] Animation blending preview
- [ ] Export to standard formats (FBX, COLLADA, glTF)
- [ ] Import from standard formats
- [ ] Batch processing tools
- [ ] Animation retargeting
- [ ] Collision mesh visualization
- [ ] Level of Detail (LOD) support

## Troubleshooting

### OpenGL Issues
If you encounter OpenGL errors, ensure you have proper graphics drivers installed:
- Linux: Install Mesa or proprietary drivers
- Windows: Update graphics drivers
- macOS: Should work out of the box (macOS 10.9+)

### File Loading Errors
- Ensure file paths are correct
- Check that files are from the PC version of THUG
- Verify file formats match the documentation

### Performance Issues
- Try reducing viewport resolution
- Disable wireframe overlay
- Close other 3D applications

## Contributing

Contributions are welcome! Please see the main repository's `CONTRIBUTING.md` for guidelines.

## License

This tool is part of the THUG source code repository and is provided for educational and preservation purposes. See the main repository's `LEGAL.md` for details.

## Credits

- Original THUG character models and animations: Neversoft Entertainment
- Tool development: THUG preservation community
