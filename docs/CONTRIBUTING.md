# Contributing to THUG Source Code

## Welcome!

Thank you for your interest in contributing to the Tony Hawk's Underground source code repository! This document provides guidelines for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Process](#development-process)
- [Coding Standards](#coding-standards)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)
- [Community](#community)

## Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors, regardless of experience level, background, or identity.

### Expected Behavior

- Be respectful and considerate in all interactions
- Welcome newcomers and help them get started
- Accept constructive criticism gracefully
- Focus on what is best for the project and community
- Show empathy towards other community members

### Unacceptable Behavior

- Harassment, discrimination, or offensive comments
- Personal attacks or trolling
- Publishing others' private information without permission
- Any conduct that would be inappropriate in a professional setting

### Enforcement

Violations of the code of conduct may result in:
- Warning from maintainers
- Temporary ban from the repository
- Permanent ban for serious or repeated violations

Report issues to the repository maintainers via GitHub issues (mark as private/sensitive).

## Getting Started

### Prerequisites

Before contributing, make sure you have:

1. **Development Environment**
   - Git installed
   - CMake 3.10 or later
   - C++ compiler (GCC 7+, Clang 6+, or MSVC 2015+)
   - Text editor or IDE (VS Code, Visual Studio, etc.)

2. **Knowledge Base**
   - Basic C++ programming
   - Understanding of game engine concepts (helpful but not required)
   - Familiarity with Git and GitHub workflow

3. **Documentation Read**
   - [README.md](../README.md) - Project overview
   - [BUILDING.md](../BUILDING.md) - Build instructions
   - [ROADMAP.md](../ROADMAP.md) - Development roadmap
   - [CODE_STYLE.md](CODE_STYLE.md) - Coding standards

### Setting Up Your Development Environment

1. **Fork the Repository**
   ```bash
   # Click "Fork" on GitHub, then clone your fork:
   git clone https://github.com/YOUR_USERNAME/thug.git
   cd thug
   ```

2. **Add Upstream Remote**
   ```bash
   git remote add upstream https://github.com/kamer1337/thug.git
   git fetch upstream
   ```

3. **Build the Project**
   ```bash
   mkdir build
   cd build
   cmake ..
   # Note: Full build may not work due to missing implementations
   # but this verifies your setup
   ```

4. **Explore the Codebase**
   ```bash
   # Review the code structure
   ls -R Code/
   
   # Check documentation
   ls docs/
   ```

## How to Contribute

### Types of Contributions

We welcome various types of contributions:

#### 1. Documentation (Easy - Great for Beginners!)

- **Improve Existing Documentation**
  - Fix typos and grammatical errors
  - Clarify confusing sections
  - Add missing information

- **Create New Documentation**
  - Document subsystems (see [docs/subsystems/](subsystems/))
  - Write tutorials (see [docs/tutorials/](tutorials/))
  - Create architecture diagrams
  - Add code examples

- **API Documentation**
  - Document classes and functions
  - Add usage examples
  - Explain algorithms and data structures

**Good First Issues**: Look for documentation TODOs in ROADMAP.md

#### 2. Code Comments (Easy)

- Add comments to complex code sections
- Explain algorithms and techniques
- Document function purposes and parameters
- Clarify non-obvious logic

```cpp
// Before
void UpdatePhysics(float dt)
{
    v.y -= 9.81f * dt;
    p += v * dt;
}

// After
void UpdatePhysics(float dt)
{
    // Apply gravity acceleration (9.81 m/s²)
    v.y -= 9.81f * dt;
    
    // Update position using Euler integration
    p += v * dt;
}
```

#### 3. Code Analysis (Medium)

- Analyze game engine architecture
- Document algorithms and techniques
- Create flowcharts and diagrams
- Compare with modern game engines
- Write technical articles

#### 4. Bug Fixes (Medium)

- Fix compilation warnings
- Fix obvious bugs in existing code
- Improve error handling
- Fix platform-specific issues

**Note**: Always test your fixes thoroughly!

#### 5. Feature Implementation (Hard)

- Implement stub functions (see [docs/platforms/STUB_FUNCTIONS.md](platforms/STUB_FUNCTIONS.md))
- Complete Win32 implementations
- Add missing functionality
- Port subsystems to modern APIs

**Tip**: Start with simple stubs and work up to complex systems

#### 6. Testing (Medium)

- Create unit tests for existing code
- Add integration tests
- Document test procedures
- Verify cross-platform compatibility

#### 7. Build System Improvements (Medium)

- Improve CMake configuration
- Add build targets
- Fix platform-specific build issues
- Create CI/CD workflows

#### 8. Tools and Utilities (Medium-Hard)

- Asset extraction tools
- Analysis utilities
- Debugging tools
- Performance profilers

## Development Process

### Workflow

1. **Check Existing Issues/PRs**
   - Search for related issues or pull requests
   - Comment if you want to work on an issue
   - Avoid duplicate work

2. **Create an Issue (Optional but Recommended)**
   - Describe what you want to contribute
   - Discuss approach with maintainers
   - Get feedback before investing significant time

3. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/bug-description
   # or
   git checkout -b docs/documentation-update
   ```

4. **Make Changes**
   - Follow coding standards ([CODE_STYLE.md](CODE_STYLE.md))
   - Write clear commit messages
   - Test your changes
   - Document as you go

5. **Commit Changes**
   ```bash
   git add .
   git commit -m "Add feature: brief description"
   ```

6. **Push to Your Fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create Pull Request**
   - Go to GitHub and create a PR
   - Fill out the PR template
   - Link related issues
   - Wait for review

### Commit Messages

Write clear, descriptive commit messages:

```
Add Win32 texture loading implementation

- Implement CTexture::Load() for DirectX 8
- Add error handling for file I/O
- Support multiple texture formats
- Update documentation in WIN32.md

Fixes #123
```

Format:
- **First line**: Brief summary (50 chars or less)
- **Blank line**
- **Body**: Detailed description (wrap at 72 chars)
  - What changed
  - Why it changed
  - Any relevant context
- **Footer**: Reference issues (Fixes #123, Closes #456)

### Testing Your Changes

Before submitting a PR:

1. **Build Test**
   ```bash
   cd build
   cmake ..
   make  # or cmake --build .
   ```

2. **Code Review**
   - Review your own changes
   - Check for typos and errors
   - Verify formatting

3. **Documentation**
   - Update relevant documentation
   - Add comments to code
   - Update CHANGES.md if appropriate

4. **Sanity Check**
   - Does it build without errors?
   - Does it introduce warnings?
   - Does it break existing functionality?

## Coding Standards

### Follow the Style Guide

- Read [CODE_STYLE.md](CODE_STYLE.md)
- Match existing code style
- Use tabs for indentation
- Follow naming conventions

### Key Points

```cpp
// Good
class CExample
{
public:
    void UpdateState();
    
private:
    int m_value;
};

// Bad
class example {
  public:
    void update_state();
  private:
    int value;
};
```

### Code Quality

- **No compiler warnings** (or document why they're unavoidable)
- **Const correctness** where appropriate
- **Error handling** for failure cases
- **Memory safety** (no leaks, no buffer overruns)
- **Clear logic** (readable and maintainable)

## Documentation

### Documentation Standards

1. **Markdown Format**
   - Use standard Markdown syntax
   - Include table of contents for long documents
   - Use code blocks with language tags

2. **Clear Structure**
   - Hierarchical headings
   - Short paragraphs
   - Bullet points for lists
   - Code examples where relevant

3. **File Placement**
   - Subsystem docs: `docs/subsystems/`
   - Platform docs: `docs/platforms/`
   - Tutorials: `docs/tutorials/`
   - General docs: `docs/`

4. **Cross-References**
   - Link to related documentation
   - Use relative paths
   - Keep links up to date

### Example Documentation

````markdown
# System Name

## Overview
Brief description of what this system does.

## Architecture
Explain the design and structure.

## Usage
```cpp
// Code example
CSystem system;
system.Initialize();
```

## Related
- [Other System](OTHER_SYSTEM.md)
- [Architecture](../ARCHITECTURE.md)
````

## Pull Request Process

### Before Creating a PR

- [ ] Code builds without errors
- [ ] Changes follow coding standards
- [ ] Documentation is updated
- [ ] Commit messages are clear
- [ ] Changes are focused and minimal

### PR Description Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation
- [ ] Code cleanup
- [ ] Build system
- [ ] Other (specify)

## Changes Made
- List specific changes
- One per line
- Be clear and concise

## Testing
How did you test these changes?

## Related Issues
Fixes #123
Relates to #456

## Checklist
- [ ] Code follows style guidelines
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Changes are minimal and focused
```

### Review Process

1. **Automated Checks** (if configured)
   - Build verification
   - Static analysis
   - Format checking

2. **Maintainer Review**
   - Code quality
   - Design decisions
   - Documentation
   - Testing

3. **Discussion**
   - Respond to feedback
   - Make requested changes
   - Ask questions if unclear

4. **Approval and Merge**
   - Maintainer approves
   - PR is merged
   - Branch can be deleted

### After Your PR is Merged

- Pull the latest changes
  ```bash
  git checkout main
  git pull upstream main
  ```
- Delete your feature branch
  ```bash
  git branch -d feature/your-feature-name
  ```
- Celebrate! 🎉

## Community

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests, discussions
- **GitHub Discussions**: General questions and community chat
- **Pull Requests**: Code contributions and reviews

### Getting Help

- **Documentation**: Check docs/ directory first
- **Issues**: Search existing issues for answers
- **Ask**: Open a discussion or issue if you need help

### Recognition

Contributors are recognized in:
- Git commit history
- Pull request acknowledgments
- Project documentation
- Community thanks

## Specific Contribution Areas

### Documentation Contributors

Focus areas:
- Subsystem documentation
- API reference
- Tutorials and guides
- Architecture diagrams

See: [ROADMAP.md](../ROADMAP.md) Section 4.1

### Code Contributors

Focus areas:
- Stub function implementation
- Bug fixes
- Code modernization
- Platform support

See: [docs/platforms/STUB_FUNCTIONS.md](platforms/STUB_FUNCTIONS.md)

### Analysis Contributors

Focus areas:
- Engine architecture analysis
- Algorithm documentation
- Performance analysis
- Comparative studies

See: [ROADMAP.md](../ROADMAP.md) Section 11

### Tool Developers

Focus areas:
- Asset extraction tools
- Analysis utilities
- Build improvements
- Development tools

See: [ROADMAP.md](../ROADMAP.md) Section 6

## Legal Considerations

### Copyright

- Original code © Neversoft Entertainment / Activision
- Contributions are licensed under project terms
- Don't submit copyrighted material without permission

### Attribution

- Give credit for external code or resources
- Document sources and references
- Respect intellectual property rights

### Educational Purpose

This repository is for:
- Educational use
- Historical preservation
- Research and analysis
- Learning game engine concepts

Not for:
- Commercial use
- Piracy or circumventing copy protection
- Unauthorized redistribution of game assets

## Questions?

If you have questions about contributing:

1. Check the documentation first
2. Search existing issues
3. Open a new discussion on GitHub
4. Be patient and respectful

## Thank You!

Your contributions help preserve and understand this important piece of game development history. Whether you fix a typo or implement a major feature, every contribution is valued.

Happy contributing! 🎮

---

**Last Updated**: 2024-10
**Maintained By**: Community contributors
