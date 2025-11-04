# Installation Guide - THUG Model Viewer

## System Requirements

### Operating System
- Linux (tested on Ubuntu 20.04+)
- macOS 10.14+
- Windows 10+

### Software Requirements
- Python 3.7 or higher
- pip (Python package installer)

### Hardware Requirements
- OpenGL 2.1+ compatible graphics card
- 2GB RAM minimum (4GB recommended)
- 100MB disk space

## Installation Steps

### 1. Install Python

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install python3 python3-pip
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Python
brew install python3
```

#### Windows
Download and install Python from [python.org](https://www.python.org/downloads/)
- Make sure to check "Add Python to PATH" during installation

### 2. Install Dependencies

Navigate to the tool directory:
```bash
cd tools/model_viewer
```

Install required packages:
```bash
pip3 install -r requirements.txt
```

Or install manually:
```bash
pip3 install PyQt5 PyOpenGL numpy
```

### 3. Verify Installation

Run the test suite:
```bash
python3 test_suite.py
```

You should see:
```
============================================================
✓ All tests passed!
============================================================
```

Run the examples:
```bash
python3 examples.py
```

### 4. Launch the Application

Using the launch script (recommended):
```bash
./launch.sh
```

Or directly:
```bash
python3 model_viewer.py
```

## Troubleshooting

### OpenGL Issues

**Linux:**
```bash
sudo apt install libgl1-mesa-glx libglu1-mesa
```

**macOS:**
OpenGL should be available by default. If you have issues, update macOS.

**Windows:**
Update your graphics drivers from the manufacturer's website.

### PyQt5 Issues

**Linux:**
```bash
sudo apt install python3-pyqt5 python3-pyqt5.qtopengl
```

**macOS:**
```bash
brew install pyqt5
```

### "No module named 'PyQt5'" Error

Ensure you're using the correct Python version:
```bash
python3 --version
python3 -m pip install PyQt5
```

### "libGL error" on Linux

```bash
sudo apt install libxcb-xinerama0
```

### Permission Denied on launch.sh

```bash
chmod +x launch.sh
```

## Optional Dependencies

### Pillow (for texture support)
```bash
pip3 install pillow
```

### SciPy (for advanced math)
```bash
pip3 install scipy
```

## Uninstallation

To remove the tool, simply delete the `tools/model_viewer` directory.

To remove Python dependencies:
```bash
pip3 uninstall PyQt5 PyOpenGL numpy
```

## Getting Help

If you encounter issues:
1. Check this installation guide
2. Review the main README.md
3. Run the test suite to identify problems
4. Check the GitHub issues page
5. Ensure all dependencies are up to date

## Next Steps

After installation:
1. Read the [README.md](README.md) for usage instructions
2. Try the examples: `python3 examples.py`
3. Explore the tool with: `python3 model_viewer.py`
