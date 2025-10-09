# THUG Scripting System

## Overview

Tony Hawk's Underground uses a custom scripting system called "QB" (QuickByte), a bytecode-based scripting language designed for high-performance game logic. The scripting system is central to the game's architecture, controlling gameplay, UI, animations, events, and game flow.

## Table of Contents

- [Architecture](#architecture)
- [Script Compilation](#script-compilation)
- [Script Execution](#script-execution)
- [Data Structures](#data-structures)
- [Script API](#script-api)
- [Integration with Game Engine](#integration-with-game-engine)
- [Performance Considerations](#performance-considerations)
- [Debugging](#debugging)

---

## Architecture

### System Structure

The scripting system spans two main layers:

```
┌─────────────────────────────────────────────────────┐
│          Sk/Scripting (Game-Specific Scripts)       │
│      - skfuncs.cpp: Skate-specific script functions │
│      - cfuncs.cpp: Custom game functions (518KB)    │
│      - mcfuncs.cpp: Menu/Career functions           │
│      - ftables.cpp: Function table registration     │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│         Gel/Scripting (Core Script Engine)          │
│      - script.cpp/h: Script execution engine        │
│      - parse.cpp/h: QB bytecode parser              │
│      - struct.cpp/h: Data structures (CStruct)      │
│      - array.cpp/h: Array containers                │
│      - component.cpp/h: Script components           │
│      - symboltable.cpp/h: Symbol management         │
│      - tokens.cpp/h: Token definitions              │
└─────────────────────────────────────────────────────┘
```

### Key Components

1. **Script Engine** (`Gel/Scripting/script.cpp`)
   - Manages script execution and lifecycle
   - Handles script updates and timing
   - Implements wait states and blocking behavior

2. **Parser** (`Gel/Scripting/parse.cpp`)
   - Parses QB bytecode files
   - Builds symbol tables
   - Handles data structure creation

3. **Symbol Table** (`Gel/Scripting/symboltable.cpp`)
   - Maps script names to bytecode
   - Manages global and local variables
   - Handles symbol resolution

4. **Data Structures**
   - `CStruct`: Key-value pairs (similar to dictionaries)
   - `CArray`: Dynamic arrays
   - `CComponent`: Base class for script data

---

## Script Compilation

### QB File Format

Scripts are compiled from source into QB (QuickByte) bytecode format:

1. **Source Format**: Text-based scripting language
2. **Compilation**: External tool (qcomp) compiles to bytecode
3. **Loading**: Engine parses bytecode at runtime

### Bytecode Structure

QB bytecode consists of tokens representing:

- **Keywords**: `if`, `else`, `repeat`, `begin`, `endscript`
- **Data Types**: integers, floats, strings, vectors, checksums
- **Operators**: arithmetic, logical, comparison
- **Function Calls**: Built-in and custom functions

Example token sequence:
```
ESCRIPTTOKEN_KEYWORD_SCRIPT     // Start script definition
ESCRIPTTOKEN_NAME              // Script name checksum
ESCRIPTTOKEN_KEYWORD_IF        // Conditional
ESCRIPTTOKEN_KEYWORD_ENDSCRIPT // End script
```

### Symbol Resolution

The parser creates symbol table entries:

```cpp
CSymbolTableEntry* sCreateScriptSymbol(
    uint32 nameChecksum,        // Script name (CRC32)
    uint32 contentsChecksum,    // Script content hash
    const uint8 *p_data,        // Bytecode data
    uint32 size,                // Data size
    const char *p_fileName      // Source file (debug)
);
```

---

## Script Execution

### CScript Class

The `CScript` class (`Code/Gel/Scripting/script.h`) manages individual script instances:

```cpp
class CScript : public Mem::CPoolable<CScript>
{
public:
    EScriptReturnVal Update();      // Execute script for one frame
    void Start();                   // Begin script execution
    void Stop();                    // Terminate script
    bool IsFinished();             // Check completion status
    
private:
    uint8* mp_script;              // Bytecode pointer
    CStruct* mp_params;            // Script parameters
    EWaitType m_wait_type;         // Current wait state
};
```

### Execution Model

Scripts execute in a coroutine-like model:

1. **Start**: Initialize script with parameters
2. **Update**: Execute bytecode until blocked/finished
3. **Wait**: Suspend execution (timer, event, animation)
4. **Resume**: Continue from last position
5. **Finish**: Clean up and return

### Return Values

Script updates return status codes:

```cpp
enum EScriptReturnVal
{
    ESCRIPTRETURNVAL_FINISHED,              // Script completed
    ESCRIPTRETURNVAL_BLOCKED,               // Waiting on resource
    ESCRIPTRETURNVAL_WAITING,               // Timer/event wait
    ESCRIPTRETURNVAL_ERROR,                 // Execution error
    ESCRIPTRETURNVAL_STOPPED_IN_DEBUGGER,   // Debug breakpoint
    ESCRIPTRETURNVAL_FINISHED_INTERRUPT,    // Interrupted externally
};
```

### Wait Types

Scripts can wait on various conditions:

```cpp
enum EWaitType
{
    WAIT_TYPE_NONE=0,                   // No wait, run continuously
    WAIT_TYPE_COUNTER,                  // Wait N frames
    WAIT_TYPE_TIMER,                    // Wait time duration
    WAIT_TYPE_BLOCKED,                  // Blocked on resource
    WAIT_TYPE_OBJECT_MOVE,              // Object movement complete
    WAIT_TYPE_OBJECT_ANIM_FINISHED,     // Animation complete
    WAIT_TYPE_STREAM_FINISHED,          // Audio stream complete
    WAIT_TYPE_ONE_PER_FRAME,            // Execute once per frame
};
```

### Loop Structures

Scripts support nested loops:

```cpp
#define MAX_NESTED_BEGIN_REPEATS 10

struct SLoop
{
    uint8* mp_start;      // Loop start position
    int m_count;          // Iteration count
    int m_index;          // Current iteration
};
```

---

## Data Structures

### CStruct: Key-Value Pairs

`CStruct` is the primary data container, similar to a dictionary:

```cpp
CStruct* pStruct = new CStruct();

// Add components
pStruct->AddInteger("health", 100);
pStruct->AddFloat("speed", 5.5f);
pStruct->AddChecksum("name", CRCD(0xa1dc81f9, "name"));
pStruct->AddVector("position", 10.0f, 20.0f, 30.0f);

// Retrieve components
int health;
pStruct->GetInteger("health", &health);

float speed;
pStruct->GetFloat("speed", &speed);
```

### CArray: Dynamic Arrays

`CArray` stores collections:

```cpp
CArray* pArray = new CArray();
pArray->SetSizeAndType(10, ESYMBOLTYPE_INTEGER);

// Access elements
pArray->SetInteger(0, 42);
int value = pArray->GetInteger(0);
```

### CComponent: Base Type

All script data derives from `CComponent`:

- Integers
- Floats
- Strings
- Checksums (CRC32 hashes)
- Vectors
- Pairs (key-value)
- Structures
- Arrays

---

## Script API

### Function Registration

Game functions are registered in function tables:

**File**: `Code/Sk/Scripting/ftables.cpp`

```cpp
// Register script functions
Script::RegisterScriptFunction(
    CRCD(0x12345678, "MyFunction"),
    ScriptMyFunction
);
```

### Script Function Signature

All script functions follow this signature:

```cpp
bool ScriptFunctionName(Script::CStruct *pParams, Script::CScript *pScript)
{
    // Extract parameters
    uint32 paramValue;
    pParams->GetChecksum("param_name", &paramValue);
    
    // Perform operation
    // ...
    
    // Return success/failure
    return true;
}
```

### Common Script Functions

The game provides hundreds of script functions:

**Skater Functions** (`Code/Sk/Scripting/skfuncs.cpp`):
- `ScriptForEachSkaterName()`: Iterate over skater profiles
- `ScriptGetCurrentSkaterCamAnimName()`: Get camera animation
- Trick system functions
- Animation control
- Skater state management

**Custom Functions** (`Code/Sk/Scripting/cfuncs.cpp`):
- UI management
- Level loading
- Game mode control
- Score tracking
- Career mode logic

**Menu/Career Functions** (`Code/Sk/Scripting/mcfuncs.cpp`):
- Menu system
- Career progression
- Goal management
- Save/load operations

### Example: ForEachSkaterName

```cpp
bool ScriptForEachSkaterName(Script::CStruct *pParams, Script::CScript *pScript)
{
    // Get script to execute for each skater
    uint32 scriptToRun;
    pParams->GetChecksum("do", &scriptToRun, Script::ASSERT);
    
    // Get optional parameters
    Script::CStruct* pSubParams = NULL;
    pParams->GetStructure("params", &pSubParams, Script::NO_ASSERT);
    
    // Iterate over all skater profiles
    Obj::CPlayerProfileManager* pProfileManager = 
        Mdl::Skate::Instance()->GetPlayerProfileManager();
    
    int numProfiles = pProfileManager->GetNumProfileTemplates();
    for (int i = 0; i < numProfiles; i++)
    {
        Obj::CSkaterProfile* pProfile = 
            pProfileManager->GetProfileTemplateByIndex(i);
        
        // Get skater name
        uint32 skaterName;
        pProfile->GetInfo()->GetChecksum("name", &skaterName);
        
        // Create parameters for callback script
        Script::CStruct* pTempStructure = new Script::CStruct;
        pTempStructure->AppendStructure(pSubParams);
        pTempStructure->AddChecksum("name", skaterName);
        
        // Execute callback script
        Script::RunScript(scriptToRun, pTempStructure);
        delete pTempStructure;
    }
    
    return true;
}
```

---

## Integration with Game Engine

### Script Loading

Scripts are loaded during game initialization:

1. **PreLoad**: Load QB files into memory
2. **Parse**: Convert bytecode to internal structures
3. **Register**: Add symbols to symbol table
4. **Execute**: Run initialization scripts

### Object-Script Integration

Game objects can execute scripts via components:

```cpp
class CScriptComponent : public CBaseComponent
{
    void ExecuteScript(uint32 scriptName, CStruct* pParams);
    void SpawnScript(uint32 scriptName);
    bool IsScriptRunning(uint32 scriptName);
};
```

### Event System

Scripts can respond to game events:

- Object collision
- Animation completion
- Input events
- Network messages
- Timer expiration

### Script Spawning

Scripts can spawn child scripts:

```cpp
Script::SpawnScript("MyScript", pParams);
```

This creates a new script instance that runs independently.

### Script Communication

Scripts communicate via:

1. **Global Variables**: Shared state in symbol table
2. **Return Values**: Pass data back to caller
3. **Events**: Trigger scripts on events
4. **Broadcast Messages**: Send to all scripts

---

## Performance Considerations

### Script Pooling

Scripts use object pooling to avoid allocations:

```cpp
DefinePoolableClass(Script::CScript);
```

This pre-allocates script instances for reuse.

### Execution Budget

Scripts have limited execution time per frame:

- Update stops at blocking calls
- Prevents frame rate drops
- Scripts resume next frame

### Bytecode Efficiency

QB bytecode is designed for performance:

- Compact representation (tokens)
- Fast parsing and execution
- Minimal branching in interpreter
- Cache-friendly data access

### Checksum Usage

Script names and parameters use CRC32 checksums:

```cpp
CRCD(0xa1dc81f9, "name")  // Pre-computed hash
```

This enables O(1) lookups without string comparisons.

---

## Debugging

### Script Debugger

The engine includes a script debugger (`Code/Gel/Scripting/debugger.cpp`):

**Features**:
- Set breakpoints
- Step through execution
- Inspect variables
- View call stack

**Debug Modes**:

```cpp
enum ESingleStepMode
{
    OFF=0,           // Normal execution
    WAITING,         // Paused at breakpoint
    STEP_INTO,       // Step into function calls
    STEP_OVER,       // Step over function calls
};
```

### Script Info

Scripts track execution context for debugging:

```cpp
const char* CScript::GetScriptInfo()
{
    // Returns script name, file, line number
    // Used in assertions and error messages
}
```

### Network Debugging

Multiplayer script debugging support:

**File**: `Code/Sk/GameNet/scriptdebugger.h`

- Remote debugging over network
- Synchronize breakpoints across clients
- Debug server-side scripts

### Assertions

Script functions use assertions for validation:

```cpp
pParams->GetChecksum("required_param", &value, Script::ASSERT);
```

The `Script::ASSERT` flag triggers an error if the parameter is missing.

### Common Issues

**Infinite Loops**: The parser checks for potential infinite loops:

```cpp
void CheckForPossibleInfiniteLoops(uint32 scriptName, uint8 *p_token);
```

**Missing Symbols**: Symbol table lookups log warnings for undefined scripts.

**Memory Leaks**: Scripts must clean up allocated structures and arrays.

---

## Usage Examples

### Creating and Running a Script

```cpp
// Create parameter structure
Script::CStruct* pParams = new Script::CStruct();
pParams->AddInteger("health", 100);
pParams->AddChecksum("mode", CRCD(0x12345678, "normal"));

// Run script (fire and forget)
Script::RunScript("InitializePlayer", pParams);
delete pParams;
```

### Spawning a Persistent Script

```cpp
// Spawn script that runs across frames
Script::CStruct* pParams = new Script::CStruct();
pParams->AddInteger("duration", 60);  // frames

Script::CScript* pScript = Script::SpawnScript("UpdateHUD", pParams);
delete pParams;

// Later: check if still running
if (pScript && !pScript->IsFinished())
{
    // Script still executing
}
```

### Registering a Custom Function

```cpp
// In ftables.cpp initialization
bool ScriptMyCustomFunction(Script::CStruct *pParams, Script::CScript *pScript)
{
    // Get parameters
    int value;
    if (!pParams->GetInteger("value", &value))
    {
        value = 0;  // Default
    }
    
    // Perform operation
    printf("Custom function called with value: %d\n", value);
    
    // Return result to script
    pScript->GetParams()->AddInteger("result", value * 2);
    
    return true;
}

// Register function
Script::RegisterScriptFunction(
    CRCD(0xabcdef12, "MyCustomFunction"),
    ScriptMyCustomFunction
);
```

---

## Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall engine architecture
- [GAME_LOOP.md](GAME_LOOP.md) - Frame execution and timing
- [docs/CODE_STYLE.md](CODE_STYLE.md) - Coding conventions

## Implementation Files

### Core Engine
- `Code/Gel/Scripting/script.cpp/h` - Script execution engine
- `Code/Gel/Scripting/parse.cpp/h` - QB parser
- `Code/Gel/Scripting/struct.cpp/h` - CStruct implementation
- `Code/Gel/Scripting/array.cpp/h` - CArray implementation
- `Code/Gel/Scripting/symboltable.cpp/h` - Symbol management
- `Code/Gel/Scripting/tokens.cpp/h` - Token definitions
- `Code/Gel/Scripting/debugger.cpp/h` - Script debugger

### Game Functions
- `Code/Sk/Scripting/skfuncs.cpp/h` - Skate-specific functions (276 KB)
- `Code/Sk/Scripting/cfuncs.cpp/h` - Custom game functions (518 KB)
- `Code/Sk/Scripting/mcfuncs.cpp/h` - Menu/Career functions (167 KB)
- `Code/Sk/Scripting/ftables.cpp/h` - Function table registration

---

*This document describes the scripting system as implemented in the THUG engine codebase.*
