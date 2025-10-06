# Getting Started with THUG Source Code

## Welcome!

This guide will help you get started with the Tony Hawk's Underground (THUG) source code repository. Whether you're a game developer, student, researcher, or just curious, this tutorial will guide you through understanding and working with the codebase.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [First Steps](#first-steps)
3. [Understanding the Codebase](#understanding-the-codebase)
4. [Setting Up Your Environment](#setting-up-your-environment)
5. [Building the Code](#building-the-code)
6. [Exploring the Code](#exploring-the-code)
7. [Making Your First Contribution](#making-your-first-contribution)
8. [Next Steps](#next-steps)

## Prerequisites

### What You Need to Know

**Minimum:**
- Basic programming concepts
- Familiarity with C or C++
- Basic command line usage
- Git basics (clone, commit, push)

**Helpful but not required:**
- Game engine concepts
- 3D graphics programming
- CMake build systems
- Software architecture

### Tools You'll Need

**Essential:**
- Git (version control)
- Text editor or IDE (VS Code, Visual Studio, CLion, etc.)
- CMake 3.10 or later
- C++ compiler (GCC 7+, Clang 6+, or MSVC 2015+)

**Recommended:**
- GitHub account (for contributing)
- Debugger (GDB, LLDB, or Visual Studio debugger)
- Code search tool (grep, ripgrep, or IDE search)

## First Steps

### 1. Get the Code

Clone the repository:

```bash
# Clone the repository
git clone https://github.com/kamer1337/thug.git

# Navigate into the directory
cd thug

# Look around
ls -la
```

You should see:
```
.git/               # Git repository data
.gitignore         # Files to ignore in Git
CMakeLists.txt     # Build system configuration
README.md          # Main readme
BUILDING.md        # Build instructions
CHANGES.md         # Change history
ROADMAP.md         # Development roadmap
Code/              # Source code
docs/              # Documentation
```

### 2. Read the Documentation

Start with these files in order:

1. **[../README.md](../../README.md)** - Project overview (5 min read)
   - What is THUG?
   - What's in this repository?
   - Current state of the code

2. **[../BUILDING.md](../../BUILDING.md)** - Build details (10 min read)
   - Why it won't compile as-is
   - What's needed for a full build
   - Technical barriers

3. **[../CHANGES.md](../../CHANGES.md)** - Change history (5 min read)
   - What's been done to make it buildable
   - Console code removal
   - Infrastructure added

4. **[../ROADMAP.md](../../ROADMAP.md)** - Future plans (15 min read)
   - Short-term goals
   - Medium-term goals
   - Long-term vision

### 3. Understand the Context

**What THUG Is:**
- Professional game from 2003
- Released for PS2, Xbox, GameCube, PC
- Developed by Neversoft Entertainment
- Published by Activision

**What This Repository Is:**
- PC-only source code (console code removed)
- Educational and preservation project
- Build infrastructure added
- Documentation in progress

**What This Repository Is NOT:**
- A playable game (assets not included)
- A complete, working build
- Ready for commercial use
- A reverse-engineering project

## Understanding the Codebase

### High-Level Architecture

THUG uses a layered architecture:

```
┌─────────────────────────────────────┐
│   Game Logic (Sk - Skate)          │  Game-specific code
├─────────────────────────────────────┤
│   Game Engine Layer (Gel)          │  Engine services
├─────────────────────────────────────┤
│   Graphics (Gfx)                    │  Rendering system
├─────────────────────────────────────┤
│   Core Utilities (Core)             │  Basic utilities
├─────────────────────────────────────┤
│   System Layer (Sys)                │  Platform abstraction
└─────────────────────────────────────┘
```

### Directory Structure

```
Code/
├── Core/          # Core utilities and data structures
│   ├── Debug/     # Debugging utilities
│   ├── Math/      # Math library (vectors, matrices)
│   ├── String/    # String handling
│   └── ...
│
├── Gel/           # Game Engine Layer
│   ├── Collision/ # Collision detection
│   ├── Net/       # Networking
│   ├── SoundFX/   # Sound system
│   ├── Scripting/ # Script system
│   └── ...
│
├── Gfx/           # Graphics subsystem
│   ├── DX8/       # DirectX 8 implementation
│   ├── NX/        # Graphics abstraction
│   └── ...
│
├── Sk/            # Skate game-specific code
│   ├── Objects/   # Game objects
│   ├── Modules/   # Game modules
│   └── ...
│
└── Sys/           # System/platform layer
    └── Win32/     # Windows implementation
```

### Key Concepts

#### 1. Platform Abstraction

Platform-specific code uses the `p_` prefix:

```cpp
// Platform-independent interface
// In: input.h
namespace Input
{
    void Initialize();
    void Update();
}

// Platform-specific implementation
// In: Win32/p_input.cpp
namespace Input
{
    void Initialize()
    {
        // Windows-specific initialization
    }
}
```

#### 2. Component-Based Architecture

Game objects use components:

```cpp
// Object has multiple components
CSkater
{
    CAnimationComponent m_animation;
    CPhysicsComponent m_physics;
    CModelComponent m_model;
    CInputComponent m_input;
}
```

#### 3. Scripting System

Game logic uses QB (QuickByte) scripting:

```
# Script example (not C++)
script create_skater
    CreateSkater name = Tony pos = (0, 0, 0)
    SetSkaterSpeed speed = 10
endscript
```

#### 4. Resource Management

Resources are loaded and managed:

```cpp
// Texture loading example
CTexture* pTexture = LoadTexture("skater.img");
pTexture->Upload();
pTexture->Bind();
```

## Setting Up Your Environment

### Option 1: Visual Studio Code (Recommended)

1. **Install VS Code**
   - Download from https://code.visualstudio.com/

2. **Install Extensions**
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - GitLens (optional but helpful)

3. **Open the Repository**
   ```bash
   cd thug
   code .
   ```

4. **Configure CMake**
   - Press `Ctrl+Shift+P`
   - Type "CMake: Configure"
   - Select your compiler

### Option 2: Visual Studio (Windows)

1. **Install Visual Studio**
   - Visual Studio 2019 or 2022
   - Select "Desktop development with C++"

2. **Open as CMake Project**
   - File → Open → Folder
   - Select the `thug` directory
   - VS will detect CMakeLists.txt

### Option 3: Command Line (Linux/macOS)

1. **Install Dependencies**
   ```bash
   # Ubuntu/Debian
   sudo apt install build-essential cmake git
   
   # macOS
   brew install cmake
   ```

2. **Use Your Favorite Editor**
   - Vim, Emacs, Sublime Text, etc.
   - Use command line for building

## Building the Code

### Understanding the Build

**Important**: The code will not build completely due to:
- Missing DirectX 8 SDK
- Stub functions not implemented
- Missing game assets
- Platform-specific dependencies

However, you can configure the build system:

### Step 1: Create Build Directory

```bash
cd thug
mkdir build
cd build
```

### Step 2: Configure with CMake

```bash
cmake ..
```

You'll see output like:
```
-- The C compiler identification is GNU 9.3.0
-- The CXX compiler identification is GNU 9.3.0
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/thug/build
```

### Step 3: Attempt Build (Optional)

```bash
cmake --build .
# or
make
```

**Expected Result**: Build will fail with compilation errors. This is normal and expected!

### What You've Accomplished

Even though the build fails:
- ✅ Build system is configured
- ✅ CMake successfully parsed the project
- ✅ You can see what files are included
- ✅ You can navigate the project structure

## Exploring the Code

### Exercise 1: Find a Simple Utility

Let's look at the math library:

```bash
cd Code/Core/Math
ls
```

Look for files like:
- `vector.h` - 3D vector class
- `matrix.h` - Matrix math
- `maths.h` - General math utilities

Open `vector.h` and examine:
```cpp
// Example from vector.h (simplified)
class Vec3
{
public:
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    float Length() const;
    void Normalize();
    
    Vec3 operator+(const Vec3& v) const;
    Vec3 operator*(float s) const;
};
```

**Questions to Consider:**
- What operations are supported?
- How is it similar to modern vector libraries?
- What could be improved?

### Exercise 2: Trace a System

Let's trace the input system:

1. **Find the interface**
   ```bash
   cd Code/Sys
   find . -name "*input*"
   ```

2. **Look at Win32 implementation**
   ```bash
   cd Win32
   less p_input.cpp
   ```

3. **Understand the flow**
   - How is input initialized?
   - How are keyboard events captured?
   - How is state tracked?

### Exercise 3: Understand a Component

Let's look at the animation system:

1. **Find animation code**
   ```bash
   cd Code/Gel
   ls -la | grep -i anim
   ```

2. **Read the headers**
   ```bash
   cd Animation
   ls *.h
   ```

3. **Trace dependencies**
   - What does the animation system depend on?
   - What other systems use animation?

### Exercise 4: Search for Patterns

Use grep to find patterns:

```bash
# Find all TODO comments
cd Code
grep -r "TODO" .

# Find stub functions
grep -r "Dbg_Assert(0)" .

# Find platform-specific code
find . -name "p_*.cpp"

# Find all classes with a certain pattern
grep -r "class C.*Component" .
```

## Making Your First Contribution

### Easy First Contributions

#### 1. Fix Documentation Typos

1. Find a typo in any `.md` file
2. Fix it
3. Commit: `git commit -m "Fix typo in README.md"`
4. Push and create PR

#### 2. Add Code Comments

1. Find a confusing function
2. Add explanatory comments
3. Commit: `git commit -m "Add comments to vector math"`
4. Create PR

#### 3. Document a Subsystem

1. Choose an undocumented subsystem
2. Create `docs/subsystems/SYSTEM_NAME.md`
3. Document classes, functions, purpose
4. Create PR

### Example: Adding Comments

**Before:**
```cpp
void Update(float dt)
{
    v.y -= 9.81f * dt;
    p += v * dt;
}
```

**After:**
```cpp
void Update(float dt)
{
    // Apply gravity acceleration (9.81 m/s² downward)
    v.y -= 9.81f * dt;
    
    // Update position using Euler integration
    // Note: Euler integration is simple but less accurate than
    // Runge-Kutta or Verlet integration for physics
    p += v * dt;
}
```

### Creating Your First Pull Request

1. **Fork the repository** on GitHub

2. **Clone your fork**
   ```bash
   git clone https://github.com/YOUR_USERNAME/thug.git
   cd thug
   ```

3. **Create a branch**
   ```bash
   git checkout -b docs/add-math-comments
   ```

4. **Make changes**
   - Edit files
   - Test if applicable

5. **Commit**
   ```bash
   git add .
   git commit -m "Add explanatory comments to math library"
   ```

6. **Push**
   ```bash
   git push origin docs/add-math-comments
   ```

7. **Create PR on GitHub**
   - Go to your fork on GitHub
   - Click "Compare & pull request"
   - Fill out the description
   - Submit!

## Next Steps

### Continue Learning

1. **Deep Dive into Subsystems**
   - Pick one subsystem to understand deeply
   - Read all code in that subsystem
   - Document what you learn

2. **Study Architecture Patterns**
   - Component-based design
   - Platform abstraction
   - Resource management
   - Event systems

3. **Compare with Modern Engines**
   - How does THUG compare to Unity?
   - What about Unreal Engine?
   - Modern practices vs. 2003 practices

### Contribute More

1. **Documentation**
   - Create subsystem documentation
   - Write tutorials
   - Make diagrams

2. **Analysis**
   - Analyze algorithms
   - Profile code
   - Create visualizations

3. **Implementation**
   - Implement stub functions
   - Fix bugs
   - Add features

### Advanced Topics

1. **Stub Implementation**
   - See [../platforms/STUB_FUNCTIONS.md](../platforms/STUB_FUNCTIONS.md)
   - Start with simple stubs
   - Work up to complex systems

2. **Modern Port**
   - Abstract platform layer
   - Use modern APIs (SDL2, OpenGL)
   - Create a minimal port

3. **Tools Development**
   - Asset extractors
   - Analysis tools
   - Build improvements

## Resources

### Documentation
- [Main README](../../README.md)
- [Building Guide](../../BUILDING.md)
- [Roadmap](../../ROADMAP.md)
- [Code Style Guide](../CODE_STYLE.md)
- [Contributing Guide](../CONTRIBUTING.md)

### External Resources
- [Game Engine Architecture](https://www.gameenginebook.com/)
- [Tony Hawk's Underground - Wikipedia](https://en.wikipedia.org/wiki/Tony_Hawk%27s_Underground)
- [C++ Reference](https://en.cppreference.com/)
- [CMake Documentation](https://cmake.org/documentation/)

### Community
- GitHub Issues - Ask questions
- GitHub Discussions - Chat with others
- Pull Requests - Submit contributions

## Getting Help

### Common Questions

**Q: Why won't the code build?**
A: The code is incomplete. Missing DirectX SDK, stub functions, and game assets. See [BUILDING.md](../../BUILDING.md).

**Q: Can I make the game run?**
A: Not without significant work. You'd need to implement all stub functions and add game assets.

**Q: What can I contribute?**
A: Documentation, code comments, analysis, and stub implementations. See [CONTRIBUTING.md](../CONTRIBUTING.md).

**Q: Is this legal?**
A: This is for educational and preservation purposes. Don't use for commercial purposes or piracy.

### Getting Support

1. **Search First**
   - Check documentation
   - Search existing issues

2. **Ask Questions**
   - Open a GitHub issue
   - Use GitHub Discussions

3. **Be Patient**
   - This is a community project
   - Maintainers volunteer their time

## Conclusion

You're now ready to start exploring and contributing to the THUG source code! Remember:

- **Start Small** - Small contributions are valuable
- **Learn Gradually** - Game engines are complex
- **Ask Questions** - The community is here to help
- **Have Fun** - Enjoy learning about game development!

Welcome to the community! 🎮

---

**Last Updated**: 2024-10
**Next Tutorial**: Understanding the Rendering System (coming soon)
