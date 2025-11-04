# THUG Model Viewer - Quick Reference

## Launch

```bash
cd tools/model_viewer
./launch.sh
# or
python3 model_viewer.py
```

## Keyboard Shortcuts

### File Operations
- `Ctrl+O` - Open Model
- `Ctrl+A` - Open Animation
- `Ctrl+Q` - Quit

### View Controls
- `W` - Toggle Wireframe mode
- `S` - Toggle Skeleton visibility
- `G` - Toggle Grid
- `R` - Reset Camera
- `H` - Hide/Show UI

### Animation Playback
- `Space` - Play/Pause animation
- `←` - Previous frame
- `→` - Next frame
- `Home` - First frame
- `End` - Last frame
- `+/-` - Speed up/down

## Mouse Controls

### Camera
- **Left Click + Drag** - Rotate camera (orbit)
- **Right Click + Drag** - Pan camera
- **Middle Click** - Reset camera view
- **Mouse Wheel** - Zoom in/out

### Selection
- **Click on bone** (in tree) - Select bone
- **Click in viewport** - (Future: pick bone)

## Main Interface

```
┌─────────────────────────────────────────────────────────┐
│ File  View  Help                          [Menu Bar]    │
├──────────┬────────────────────────┬─────────────────────┤
│          │                        │  Animation Info     │
│ Skeleton │                        ├─────────────────────┤
│ Tree     │    3D Viewport         │  ▶  Pause  Stop     │
│          │    (OpenGL)            │  [────●────────]    │
│ - root   │                        │  Frame: 15/30       │
│   - spine│                        │  Speed: 1.0x        │
│     - head│                       ├─────────────────────┤
│   - arms │                        │  Bone Properties    │
│          │                        │  Name: spine        │
│          │                        │  Pos: [0, 1, 0]     │
│          │                        │  Rot: [...]         │
└──────────┴────────────────────────┴─────────────────────┘
```

## Common Tasks

### Load and View a Model
1. Launch the viewer
2. File → Open Model (or Ctrl+O)
3. Select model file
4. Use mouse to orbit/zoom camera

### Play an Animation
1. Load a model with skeleton
2. File → Open Animation (or Ctrl+A)
3. Click Play button (or Space)
4. Adjust speed with +/- or speed control

### Edit Bone Transform
1. Select bone in skeleton tree
2. Modify values in properties panel
3. Changes apply in real-time

### Switch View Modes
1. Toggle wireframe: Press W
2. Toggle skeleton: Press S
3. Hide grid: Press G

## File Formats

### Currently Supported
- Test models (built-in)
- Test skeletons (built-in)

### Coming Soon
- `.mdl` - THUG model files
- `.ska` - THUG skeleton/animation files
- `.tex` - THUG texture files

## Troubleshooting

### Blank Screen
- Check OpenGL support: `glxinfo | grep OpenGL` (Linux)
- Update graphics drivers

### Slow Performance
- Reduce viewport size
- Disable wireframe overlay
- Close other 3D applications

### Controls Not Working
- Click in viewport to focus
- Check keyboard layout
- Restart application

## Python API

### Load Model
```python
from core.model_loader import ModelLoader
model = ModelLoader.load_model("path/to/model.mdl")
```

### Create Skeleton
```python
from core.skeleton import Skeleton, Bone
skeleton = Skeleton("MySkeleton")
bone = Bone("root", 0)
skeleton.add_bone(bone)
```

### Create Animation
```python
from core.animation import Animation, BoneAnimation
anim = Animation("MyAnimation")
bone_anim = BoneAnimation("root")
anim.add_bone_animation(bone_anim)
```

See `examples.py` for more detailed usage.

## Tips

- **Better Performance**: Run in release mode, not debug
- **Smoother Navigation**: Use middle mouse to reset camera
- **Bone Selection**: Click in skeleton tree for precise selection
- **Animation Scrubbing**: Drag timeline slider for frame-by-frame preview
- **Multiple Views**: (Future) Open multiple viewer windows

## Getting Help

- README.md - Full documentation
- INSTALLATION.md - Setup guide
- examples.py - Code examples
- test_suite.py - Verify installation
- GitHub Issues - Report bugs

## Version

Model Viewer v1.0
Part of THUG Source Code Preservation Project
