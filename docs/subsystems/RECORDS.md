# Records System Documentation

## Overview

The Records system manages high scores, best combos, and various performance records in Tony Hawk's Underground. It provides a hierarchical structure for storing and managing game records across different levels and game modes.

## Architecture

The Records system is organized in the following hierarchy:

```
CGameRecords (Game-wide records)
    └── CLevelRecords[] (Per-level records)
            ├── CRecordTable (High Scores)
            ├── CRecordTable (Best Combos)
            ├── CRecord (Longest Grind)
            ├── CRecord (Longest Manual)
            ├── CRecord (Longest Lip Trick)
            └── CRecord (Longest Combo)
```

## Classes

### CInitials

Stores and manages a player's 3-character initials for record entries.

**Location**: `Code/Sk/Objects/records.h`, `Code/Sk/Objects/records.cpp`

**Member Variables**:
- `m_initials[4]` - Character array storing 3 initials + null terminator

**Methods**:

#### `void Set(const char *initials)`
Sets the initials by copying the first 3 characters from the input string.

**Parameters**:
- `initials` - C-string containing the initials to set

**Example**:
```cpp
CInitials initials;
initials.Set("ABC");
```

#### `char* Get(void)`
Returns a pointer to the internal initials string.

**Returns**: Pointer to the 4-character array (3 initials + null terminator)

**Example**:
```cpp
char* playerInitials = initials.Get();
```

---

### CRecord

Represents a single record entry with initials, value, and additional metadata.

**Location**: `Code/Sk/Objects/records.h`, `Code/Sk/Objects/records.cpp`

**Member Variables**:
- `mp_initials` - Pointer to CInitials object
- `m_value` - Integer value of the record (score, time, etc.)
- `m_number` - Pro skater number or other identifier
- `m_new_record` - Flag indicating if this is a newly set record

**Constructor/Destructor**:

#### `CRecord()`
Constructor that initializes the record with default values ("XXX", 1000, 0).

#### `~CRecord()`
Destructor that cleans up the allocated CInitials object.

**Setter Methods**:

#### `void Set(const char *initials, int value, int number)`
Sets all record fields at once.

**Parameters**:
- `initials` - Player initials
- `value` - Record value
- `number` - Player/skater number

#### `void Set(CRecord *pRecord)`
Copies all fields from another CRecord.

**Parameters**:
- `pRecord` - Source record to copy from

#### `void SetInitials(const char *initials)`
Sets only the initials field.

#### `void SetValue(int value)`
Sets only the value field.

#### `void SetNumber(int number)`
Sets only the number field.

#### `void SetNewRecord(bool new_record)`
Sets the new record flag.

**Getter Methods**:

#### `char* GetInitials(void)`
Returns pointer to the initials string.

#### `int GetValue(void)`
Returns the record value.

#### `int GetNumber(void)`
Returns the player/skater number.

#### `bool GetNewRecord(void)`
Returns whether this is a new record.

**Record Comparison Methods**:

#### `bool BeatRecord(int value)`
Checks if the given value beats this record.

**Parameters**:
- `value` - Value to compare

**Returns**: `true` if value > current record value

#### `bool MaybeNewRecord(int value, int number)`
Updates the record if the new value beats the current one.

**Parameters**:
- `value` - New value to compare
- `number` - Player/skater number

**Returns**: `true` if the record was beaten and updated

**Behavior**:
- If value beats current record: updates value, number, sets new_record flag to true
- If value doesn't beat record: sets new_record flag to false

**Initials Update**:

#### `void UpdateInitials(CInitials* pInitials)`
Updates the record's initials if it was flagged as a new record.

**Parameters**:
- `pInitials` - Pointer to CInitials object to copy from

**Behavior**: Only updates if `GetNewRecord()` returns true

**Serialization**:

#### `void ReadFromStructure(Script::CStruct *pIn)`
Loads record data from a script structure (for loading from memory card).

#### `void WriteIntoStructure(Script::CStruct *pIn)`
Saves record data to a script structure (for saving to memory card).

---

### CRecordTable

Manages a table of records, such as a high score table with multiple entries.

**Location**: `Code/Sk/Objects/records.h`, `Code/Sk/Objects/records.cpp`

**Member Variables**:
- `m_numRecords` - Number of records in the table
- `mp_records` - Array of CRecord objects

**Constructor/Destructor**:

#### `CRecordTable(int num_records)`
Creates a table with the specified number of record slots.

**Parameters**:
- `num_records` - Number of records to allocate

#### `~CRecordTable()`
Destructor that cleans up the record array.

**Access Methods**:

#### `CRecord* GetRecord(int line)`
Returns a pointer to the record at the specified position.

**Parameters**:
- `line` - Index of the record (0-based)

**Returns**: Pointer to CRecord at that position

#### `int GetSize(void)`
Returns the number of records in the table.

**Table Management**:

#### `int MaybeNewEntry(int value, int number)`
Inserts a new entry into the table if it ranks high enough.

**Parameters**:
- `value` - Value to insert
- `number` - Player/skater number

**Returns**: 
- Index where entry was inserted (0-based)
- `-1` if value didn't make the table

**Behavior**:
1. Clears all new_record flags
2. Finds insertion position by comparing values
3. Shifts lower records down (losing the last entry)
4. Inserts new record with initials "XXX" and new_record flag set

**Initials Update**:

#### `void UpdateInitials(CInitials* pInitials)`
Updates initials for all records flagged as new records.

**Parameters**:
- `pInitials` - Pointer to CInitials to copy from

**Serialization**:

#### `void ReadFromStructure(Script::CStruct *pIn)`
Loads the entire table from a script structure.

#### `void WriteIntoStructure(Script::CStruct *pIn)`
Saves the entire table to a script structure.

---

### CLevelRecords

Manages all records for a specific level, including high scores, best combos, and various longest-trick records.

**Location**: `Code/Sk/Objects/records.h`, `Code/Sk/Objects/records.cpp`

**Constants** (defined in records.cpp):
- `vNumHighScores = 5` - Number of high score entries
- `vNumBestCombos = 5` - Number of best combo entries

**Member Variables**:
- `mp_highScores` - CRecordTable for high scores
- `mp_bestCombos` - CRecordTable for best combo scores
- `mp_longestGrind` - CRecord for longest grind
- `mp_longestManual` - CRecord for longest manual
- `mp_longestLipTrick` - CRecord for longest lip trick
- `mp_longestCombo` - CRecord for longest combo

**Constructor/Destructor**:

#### `CLevelRecords()`
Constructor that initializes all record tables and individual records.

**Initialization**:
- Creates high scores table with 5 entries
- Creates best combos table with 5 entries
- Creates individual records for longest tricks (initialized to 0)

#### `~CLevelRecords()`
Destructor that cleans up all allocated records.

**Accessor Methods**:

#### `CRecordTable* GetHighScores()`
Returns pointer to the high scores table.

#### `CRecordTable* GetBestCombos()`
Returns pointer to the best combos table.

#### `CRecord* GetLongestGrind()`
Returns pointer to the longest grind record.

#### `CRecord* GetLongestManual()`
Returns pointer to the longest manual record.

#### `CRecord* GetLongestLipTrick()`
Returns pointer to the longest lip trick record.

#### `CRecord* GetLongestCombo()`
Returns pointer to the longest combo record.

**Initials Update**:

#### `void UpdateInitials(CInitials* pInitials)`
Updates initials for all records (both tables and individual records).

**Serialization**:

#### `void ReadFromStructure(Script::CStruct *pIn)`
Loads all level records from a script structure.

**Expected Structure**:
- `HighScores` - Structure containing high score table
- `BestCombos` - Structure containing best combo table
- `LongestCombo` - Structure containing longest combo record
- `LongestGrind` - Structure containing longest grind record
- `LongestLipTrick` - Structure containing longest lip trick record
- `LongestManual` - Structure containing longest manual record

#### `void WriteIntoStructure(Script::CStruct *pIn)`
Saves all level records to a script structure.

---

### CGameRecords

Top-level class managing records for all levels in the game, plus default initials.

**Location**: `Code/Sk/Objects/records.h`, `Code/Sk/Objects/records.cpp`

**Member Variables**:
- `m_numLevels` - Number of levels in the game
- `mp_defaultInitials` - Default initials for new records
- `mp_levelRecords` - Array of CLevelRecords (one per level)

**Constructor/Destructor**:

#### `CGameRecords(int numLevels)`
Creates the game records structure for the specified number of levels.

**Parameters**:
- `numLevels` - Number of levels in the game

**Initialization**:
- Allocates level records array
- Sets default initials to "ABC"

#### `~CGameRecords()`
Destructor that cleans up the level records array and default initials.

**Default Initials**:

#### `CInitials* GetDefaultInitials(void)`
Returns pointer to the default initials object.

#### `void SetDefaultInitials(const char *p_initials)`
Sets the default initials used for new records.

**Parameters**:
- `p_initials` - New default initials

**Level Access**:

#### `CLevelRecords* GetLevelRecords(int level)`
Returns pointer to the records for a specific level.

**Parameters**:
- `level` - Level index (0-based)

**Returns**: Pointer to CLevelRecords for that level

**Serialization**:

#### `void ReadFromStructure(Script::CStruct *pIn)`
Loads all game records from a script structure (memory card loading).

**Expected Structure**:
- `GameRecords` - Array of level record structures
- `DefaultInitials` - String containing default initials

#### `void WriteIntoStructure(Script::CStruct *pIn)`
Saves all game records to a script structure (memory card saving).

---

## Usage Examples

### Creating and Using Records

```cpp
// Create game records for 10 levels
CGameRecords* pGameRecords = new CGameRecords(10);

// Set default initials
pGameRecords->SetDefaultInitials("SKT");

// Get records for level 0
CLevelRecords* pLevelRecords = pGameRecords->GetLevelRecords(0);

// Try to add a high score
int score = 125000;
int playerNumber = 0;
int position = pLevelRecords->GetHighScores()->MaybeNewEntry(score, playerNumber);

if (position >= 0) {
    // It's a new high score!
    // Update the initials
    pLevelRecords->GetHighScores()->UpdateInitials(pGameRecords->GetDefaultInitials());
}
```

### Checking Individual Records

```cpp
// Try to beat longest grind record
int grindTime = 5000; // in hundredths of a second
CRecord* pGrindRecord = pLevelRecords->GetLongestGrind();

if (pGrindRecord->MaybeNewRecord(grindTime, 0)) {
    // New record set!
    CInitials* pInitials = pGameRecords->GetDefaultInitials();
    pGrindRecord->UpdateInitials(pInitials);
}
```

### Saving and Loading

```cpp
// Save to memory card
Script::CStruct* pSaveStruct = new Script::CStruct;
pGameRecords->WriteIntoStructure(pSaveStruct);
// ... save pSaveStruct to memory card ...

// Load from memory card
Script::CStruct* pLoadStruct = /* ... load from memory card ... */;
pGameRecords->ReadFromStructure(pLoadStruct);
```

## Integration with Game Systems

The Records system is integrated with the game through the `Mdl::Skate` module:

### UpdateRecords Flow

Located in `Code/Sk/Modules/Skate/skate.cpp`:

1. Called after a skating session ends
2. Retrieves current level and score information
3. Attempts to insert scores/times into appropriate tables:
   - Total score → High Scores table
   - Best combo → Best Combos table
   - Longest grind → Longest Grind record
   - Longest manual → Longest Manual record
   - Longest lip trick → Longest Lip Trick record
   - Longest combo → Longest Combo record
4. Sets `m_new_record` flag if any records were beaten
5. Later, initials are updated through the UpdateInitials mechanism

## Memory Management

**Important Notes**:
- All classes use dynamic allocation for nested objects
- Proper cleanup is handled in destructors
- Arrays are deleted with `delete[]`, single objects with `delete`
- The hierarchical structure ensures proper cleanup when top-level objects are destroyed

## File Locations

**Header**: `Code/Sk/Objects/records.h`
**Implementation**: `Code/Sk/Objects/records.cpp`
**Usage**: `Code/Sk/Modules/Skate/skate.cpp` (UpdateRecords function)

## Dependencies

- `Core/Defines.h` - Core type definitions
- `Gel/Scripting/script.h` - Script system
- `Gel/Scripting/struct.h` - Script structures
- `Gel/Scripting/array.h` - Script arrays

## Design Notes

From comments in `records.h`:
- Uses dynamic containers for flexibility
- Number of entries can be decided at runtime
- No cross-includes needed between header files
- Trade-off: Uses more memory, potentially slower, but more flexible

## Constants

Defined in `records.cpp`:
```cpp
enum {
    vNumHighScores = 5,  // Number of high score entries per level
    vNumBestCombos = 5,  // Number of best combo entries per level
};
```

These values determine how many entries are stored in the high score and best combo tables for each level.
