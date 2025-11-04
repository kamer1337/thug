#!/bin/bash
# Comprehensive verification script

set -e

echo "=========================================="
echo "THUG Model Viewer - Verification Script"
echo "=========================================="
echo ""

# Check file structure
echo "Checking file structure..."
for file in README.md INSTALLATION.md QUICKSTART.md requirements.txt model_viewer.py launch.sh test_suite.py examples.py; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file - MISSING!"
        exit 1
    fi
done

for dir in core ui utils; do
    if [ -d "$dir" ]; then
        echo "  ✓ $dir/"
    else
        echo "  ✗ $dir/ - MISSING!"
        exit 1
    fi
done

# Check Python files
echo ""
echo "Checking Python modules..."
for module in core/skeleton.py core/animation.py core/model_loader.py core/camera.py core/renderer.py; do
    if [ -f "$module" ]; then
        echo "  ✓ $module"
    else
        echo "  ✗ $module - MISSING!"
        exit 1
    fi
done

# Check UI files
echo ""
echo "Checking UI components..."
for module in ui/main_window.py ui/viewport.py ui/animation_panel.py ui/skeleton_tree.py ui/properties_panel.py; do
    if [ -f "$module" ]; then
        echo "  ✓ $module"
    else
        echo "  ✗ $module - MISSING!"
        exit 1
    fi
done

# Run tests
echo ""
echo "Running test suite..."
python3 test_suite.py

# Run examples
echo ""
echo "Running examples..."
if python3 examples.py > /tmp/examples_output.txt 2>&1; then
    echo "  ✓ Examples completed successfully"
else
    echo "  ✗ Examples failed!"
    cat /tmp/examples_output.txt
    exit 1
fi

echo ""
echo "=========================================="
echo "✓ Verification Complete!"
echo "=========================================="
echo ""
echo "File structure: OK"
echo "Python modules: OK"
echo "UI components: OK"
echo "Tests: PASSED"
echo "Examples: PASSED"
echo ""
echo "The Model Viewer is ready to use!"
echo ""
