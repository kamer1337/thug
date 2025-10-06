# Code Style Guide

## Overview

This document defines the coding standards and style guidelines for the Tony Hawk's Underground source code repository. While the original code follows conventions from 2003-era C++, this guide helps maintain consistency when adding new code or making modifications.

## Goals

1. **Preserve Original Style** - Maintain consistency with existing codebase
2. **Readability** - Code should be easy to read and understand
3. **Maintainability** - Code should be easy to modify and extend
4. **Historical Accuracy** - Respect the original design patterns

## General Principles

### 1. When in Rome...

When modifying existing files, **match the existing style** in that file. Consistency within a file is more important than following these guidelines exactly.

### 2. Modernization with Care

When adding new code:
- Use modern C++ features where appropriate
- Don't break existing code patterns
- Document any deviations from the original style

### 3. Clarity Over Cleverness

- Write clear, straightforward code
- Avoid overly clever or obscure constructs
- Comment complex algorithms

## File Organization

### Header Files (.h)

Standard header file structure:

```cpp
/*****************************************************************************
**                                                                          **
** Filename: FileName.h                                                     **
**                                                                          **
** Description: Brief description of what this header defines               **
**                                                                          **
** Copyright (c) [Year] [Company]                                          **
**                                                                          **
*****************************************************************************/

#ifndef __FILENAME_H
#define __FILENAME_H

// System includes
#include <stdio.h>

// Core includes
#include "Core/Defines.h"

// Forward declarations
namespace Obj
{
    class CObject;
}

// Namespace
namespace Sys
{

// Constants
const int MAX_OBJECTS = 100;

// Class declaration
class CSystemClass
{
public:
    CSystemClass();
    ~CSystemClass();
    
    void Update();
    
private:
    int m_value;
};

} // namespace Sys

#endif // __FILENAME_H
```

### Source Files (.cpp)

Standard source file structure:

```cpp
/*****************************************************************************
**                                                                          **
** Filename: FileName.cpp                                                   **
**                                                                          **
** Description: Implementation of functionality described in FileName.h     **
**                                                                          **
** Copyright (c) [Year] [Company]                                          **
**                                                                          **
*****************************************************************************/

// Header for this file
#include "FileName.h"

// Other includes
#include "Core/Debug.h"

// Namespace
namespace Sys
{

// Static/local variables
static int s_instanceCount = 0;

// Function implementations
CSystemClass::CSystemClass()
    : m_value(0)
{
    s_instanceCount++;
}

CSystemClass::~CSystemClass()
{
    s_instanceCount--;
}

void CSystemClass::Update()
{
    // Implementation
}

} // namespace Sys
```

## Naming Conventions

### Files

- Header files: `CamelCase.h` (e.g., `SkateObject.h`)
- Source files: `CamelCase.cpp` (e.g., `SkateObject.cpp`)
- Platform-specific: `p_filename.cpp` (e.g., `p_display.cpp`)
- Test files: `test_filename.cpp` (e.g., `test_math.cpp`)

### Classes

```cpp
// Classes use CamelCase with 'C' prefix
class CSkater
{
};

// Structs use CamelCase with 'S' prefix (or no prefix for POD types)
struct SSkaterData
{
    int score;
    float speed;
};
```

### Functions and Methods

```cpp
// Methods use CamelCase (no prefix)
void UpdatePosition();

// Static functions use CamelCase
static void CalculateScore();

// Getters/Setters use Get/Set prefix
int GetScore() const;
void SetScore(int score);
```

### Variables

```cpp
// Member variables use m_ prefix
class CExample
{
private:
    int m_health;
    float m_speed;
    CObject* mp_object;  // Pointer: mp_ prefix
};

// Local variables use camelCase (first letter lowercase)
void Function()
{
    int localValue = 0;
    float mySpeed = 10.0f;
}

// Static variables use s_ prefix
static int s_globalCount = 0;

// Global variables use g_ prefix (avoid if possible)
int g_screenWidth = 800;

// Constants use ALL_CAPS or kConstantName
const int MAX_PLAYERS = 4;
const float kGravity = 9.81f;

// Enums use E prefix and UPPER_CASE values
enum EState
{
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING
};
```

### Namespaces

```cpp
// Namespaces use CamelCase
namespace Gfx
{
    class CTexture;
}

namespace Gel
{
    namespace SoundFX
    {
        class CSound;
    }
}
```

## Formatting

### Indentation

- Use **tabs** for indentation (matching original codebase)
- Tab width: 4 spaces (for display)
- Never mix tabs and spaces

```cpp
void Function()
{
	if (condition)
	{
		DoSomething();
	}
}
```

### Braces

- Opening brace on new line (Allman style)
- Always use braces, even for single-line statements

```cpp
// Good
if (condition)
{
    DoSomething();
}

// Also acceptable for very short statements in original code
if (condition)
    DoSomething();

// Bad - avoid in new code
if (condition) DoSomething();
```

### Line Length

- Maximum 100 characters per line (guideline, not strict)
- Break long lines for readability

```cpp
// Good
void LongFunction(int parameter1, int parameter2,
                  int parameter3, int parameter4)
{
    // Implementation
}

// Good
int result = CalculateValue(longParameterName,
                           anotherLongParameterName,
                           yetAnotherParameter);
```

### Spacing

```cpp
// Spaces around operators
int result = a + b * c;

// Space after keywords
if (condition)
while (condition)
for (int i = 0; i < count; i++)

// No space between function name and parentheses
Function();
DoSomething(param);

// Space after comma in parameter lists
Function(a, b, c);

// No space inside parentheses (usually)
if (condition && (subCondition || otherCondition))
```

### Pointer and Reference Alignment

```cpp
// Pointer/reference attached to type (preferred in most of codebase)
int* pointer;
int& reference;

// Or attached to variable (also seen in codebase)
int *pointer;
int &reference;

// Be consistent within each file
```

## Comments

### File Headers

Every file should have a header comment block (see File Organization section above).

### Function Comments

```cpp
/******************************************************************/
/* Function: CalculateScore                                       */
/* Description: Calculates the player's score based on tricks    */
/* Parameters:                                                    */
/*   tricks - Array of tricks performed                          */
/*   count - Number of tricks in array                           */
/* Returns: Total score                                          */
/******************************************************************/
int CalculateScore(Trick* tricks, int count)
{
    // Implementation
}
```

Or simpler style:

```cpp
// Calculate the player's score based on tricks performed
// Returns the total score, or 0 if no tricks
int CalculateScore(Trick* tricks, int count)
{
    // Implementation
}
```

### Inline Comments

```cpp
// Use inline comments to explain complex logic
if (velocity.LengthSquared() > kMaxSpeedSquared)
{
    // Clamp velocity to max speed while preserving direction
    velocity.Normalize();
    velocity *= kMaxSpeed;
}

// Avoid obvious comments
x = 0;  // Bad: Set x to zero (this is obvious)
```

### TODO Comments

```cpp
// TODO: Implement collision detection
// FIXME: This crashes when count is zero
// HACK: Temporary workaround for rendering bug
// NOTE: This must match the server-side calculation
```

## Language Features

### Includes

Order includes from most specific to most general:

```cpp
// 1. Header for this .cpp file
#include "ThisFile.h"

// 2. Project headers
#include "Sk/SkateObject.h"
#include "Gel/Physics.h"

// 3. System headers
#include <stdio.h>
#include <string.h>
```

### Namespaces

```cpp
// Don't use "using namespace" in headers
// In headers, always fully qualify
namespace Gfx
{
    Obj::CObject* GetObject();  // Fully qualified
}

// In .cpp files, you can use namespace declarations
using namespace Sk;
using Obj::CObject;

// Or fully qualify
Sk::CSkater* skater = new Sk::CSkater();
```

### Classes

```cpp
class CExample
{
public:
    // Public interface first
    CExample();
    ~CExample();
    
    void PublicMethod();
    
protected:
    // Protected members
    void ProtectedMethod();
    
private:
    // Private implementation details last
    void PrivateMethod();
    
    // Member variables last
    int m_value;
};
```

### Constructors

```cpp
// Use initialization lists
CExample::CExample()
    : m_value(0)
    , m_speed(1.0f)
    , mp_object(nullptr)
{
    // Constructor body
}

// For multiple inheritance or long lists, one per line
CComplexClass::CComplexClass()
    : CBaseClass()
    , m_firstValue(0)
    , m_secondValue(0)
    , m_thirdValue(0)
{
}
```

### Const Correctness

```cpp
// Mark const methods as const
int GetValue() const { return m_value; }

// Use const parameters where appropriate
void SetName(const char* name);
void ProcessData(const Data& data);

// Use const for read-only pointers
void Display(const CObject* pObject);
```

### Error Handling

```cpp
// Use assertions for programming errors
Dbg_Assert(pointer != nullptr);
Dbg_MsgAssert(value >= 0, ("Value must be non-negative"));

// Use return values for runtime errors
bool LoadFile(const char* filename)
{
    if (!FileExists(filename))
    {
        return false;
    }
    // Load file
    return true;
}

// Check and handle errors
if (!LoadFile("data.dat"))
{
    Dbg_Warning("Failed to load data file");
    // Handle error
}
```

## Platform-Specific Code

### Preprocessor Directives

```cpp
// Platform detection
#if defined(__PLAT_WN32__)
    // Windows code
    #include <windows.h>
#elif defined(__PLAT_LINUX__)
    // Linux code
    #include <unistd.h>
#else
    #error "Unsupported platform"
#endif

// Feature detection
#ifdef HAVE_NETWORKING
    InitializeNetwork();
#endif
```

### Platform Abstraction

```cpp
// Use platform-specific files with p_ prefix
// p_display.cpp for Windows
// p_display_linux.cpp for Linux

// Keep platform-independent interface in shared header
// display.h (shared interface)
namespace Sys
{
    void InitDisplay();
    void UpdateDisplay();
}
```

## Best Practices

### Memory Management

```cpp
// Always pair new/delete
CObject* pObject = new CObject();
// Use object
delete pObject;

// Null pointers after deletion
pObject = nullptr;

// Check pointers before use
if (pObject)
{
    pObject->Update();
}

// Use smart pointers in new code (if using C++11+)
std::unique_ptr<CObject> pObject(new CObject());
```

### Arrays and Loops

```cpp
// Use const for array sizes
const int MAX_ITEMS = 10;
Item items[MAX_ITEMS];

// Preferred loop style
for (int i = 0; i < MAX_ITEMS; i++)
{
    items[i].Update();
}

// Range-based for (if using C++11+)
for (auto& item : items)
{
    item.Update();
}
```

### Switch Statements

```cpp
switch (state)
{
    case STATE_IDLE:
        // Handle idle
        break;
        
    case STATE_RUNNING:
        // Handle running
        break;
        
    case STATE_JUMPING:
        // Handle jumping
        break;
        
    default:
        Dbg_MsgAssert(0, ("Unknown state: %d", state));
        break;
}
```

### Function Length

- Keep functions short and focused
- Extract complex logic into helper functions
- Maximum ~50 lines per function (guideline)

```cpp
// Good - focused function
void UpdatePlayer()
{
    UpdatePosition();
    UpdateAnimation();
    UpdatePhysics();
    CheckCollisions();
}

// Better than one massive function
```

## Documentation

### Header Documentation

Document public APIs in header files:

```cpp
/**
 * CSkater - Represents a skater character
 * 
 * This class manages a skater's state, animation, and physics.
 * It handles input processing and trick execution.
 */
class CSkater
{
public:
    /**
     * Update the skater state
     * @param deltaTime - Time elapsed since last update (in seconds)
     */
    void Update(float deltaTime);
    
    /**
     * Execute a trick
     * @param trickId - ID of the trick to perform
     * @return true if trick was successfully started
     */
    bool DoTrick(int trickId);
};
```

### Implementation Comments

```cpp
void CSkater::Update(float deltaTime)
{
    // Update position based on velocity
    // Using Euler integration for simplicity
    m_position += m_velocity * deltaTime;
    
    // Apply gravity when in air
    if (!IsOnGround())
    {
        m_velocity.y -= kGravity * deltaTime;
    }
    
    // Complex algorithm follows...
    // [Explain complex sections]
}
```

## Modern C++ Features

When adding new code with C++11/14/17 features:

### Auto Keyword
```cpp
// Use auto for complex types
auto it = map.find(key);
auto result = ComplexFunction();

// Don't overuse - be clear about types
int count = GetCount();  // Better than auto when obvious
```

### nullptr
```cpp
// Use nullptr instead of NULL
CObject* pObject = nullptr;

// Check against nullptr
if (pObject != nullptr)
{
    pObject->Update();
}
```

### Range-based For
```cpp
// Use for collections
for (const auto& item : collection)
{
    item.Process();
}
```

### Override Keyword
```cpp
class CDerived : public CBase
{
public:
    void Update() override;  // Makes intent clear
};
```

## Code Review Checklist

Before submitting code:

- [ ] Matches existing style in the file
- [ ] Proper indentation (tabs)
- [ ] Meaningful variable/function names
- [ ] Comments explain complex logic
- [ ] No compiler warnings
- [ ] Const correctness
- [ ] Error handling present
- [ ] Memory management correct (no leaks)
- [ ] Platform-specific code properly abstracted
- [ ] Documentation updated

## Tools

### Recommended Editor Settings

For Visual Studio Code:
```json
{
    "editor.insertSpaces": false,
    "editor.tabSize": 4,
    "editor.detectIndentation": false,
    "files.trimTrailingWhitespace": true
}
```

For Visual Studio:
- Tools → Options → Text Editor → C/C++ → Tabs
- Tab size: 4, Indent size: 4, Insert tabs

### Formatting Tools

Consider using `.clang-format` for automated formatting:
```yaml
# .clang-format
BasedOnStyle: Microsoft
IndentWidth: 4
UseTab: Always
TabWidth: 4
BreakBeforeBraces: Allman
ColumnLimit: 100
PointerAlignment: Left
```

## Related Documentation

- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
- [../BUILDING.md](../BUILDING.md) - Build instructions
- [../ROADMAP.md](../ROADMAP.md) - Development roadmap

---

**Remember**: Consistency is more important than any specific rule. When in doubt, match the surrounding code.

**Last Updated**: 2024-10
