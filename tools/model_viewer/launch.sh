#!/bin/bash
# Launch script for THUG Model Viewer
# This script checks dependencies and launches the application

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "THUG Model Viewer - Launch Script"
echo "=================================="
echo ""

# Check Python version
echo -n "Checking Python version... "
python_version=$(python3 --version 2>&1 | awk '{print $2}')
python_major=$(echo $python_version | cut -d. -f1)
python_minor=$(echo $python_version | cut -d. -f2)

if [ "$python_major" -lt 3 ] || ([ "$python_major" -eq 3 ] && [ "$python_minor" -lt 7 ]); then
    echo -e "${RED}FAIL${NC}"
    echo "Python 3.7+ required, found $python_version"
    exit 1
fi
echo -e "${GREEN}OK${NC} ($python_version)"

# Check if dependencies are installed
echo -n "Checking dependencies... "
missing_deps=()

if ! python3 -c "import PyQt5" 2>/dev/null; then
    missing_deps+=("PyQt5")
fi

if ! python3 -c "import OpenGL" 2>/dev/null; then
    missing_deps+=("PyOpenGL")
fi

if ! python3 -c "import numpy" 2>/dev/null; then
    missing_deps+=("numpy")
fi

if [ ${#missing_deps[@]} -gt 0 ]; then
    echo -e "${RED}MISSING${NC}"
    echo ""
    echo "Missing dependencies: ${missing_deps[*]}"
    echo ""
    echo "Install them with:"
    echo "  pip install -r requirements.txt"
    echo ""
    echo "Or install individually:"
    for dep in "${missing_deps[@]}"; do
        echo "  pip install $dep"
    done
    exit 1
fi
echo -e "${GREEN}OK${NC}"

# Launch the application
echo ""
echo "Launching Model Viewer..."
echo ""

python3 model_viewer.py "$@"
