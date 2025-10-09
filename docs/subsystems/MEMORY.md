# THUG Memory Management System

## Overview

Tony Hawk's Underground uses a sophisticated custom memory management system designed for console gaming. The system provides fine-grained control over memory allocation, pooling, regions, and debugging, essential for optimizing performance on memory-constrained platforms.

## Table of Contents

- [Architecture](#architecture)
- [Memory Regions](#memory-regions)
- [Allocators](#allocators)
- [Memory Pools](#memory-pools)
- [Poolable Objects](#poolable-objects)
- [Memory Budgets](#memory-budgets)
- [Debugging Tools](#debugging-tools)
- [Best Practices](#best-practices)

---

## Architecture

### System Structure

The memory management system is organized hierarchically:

```
┌─────────────────────────────────────────────────────┐
│         Mem::Manager (Memory Manager)               │
│      Code/Sys/Mem/memman.h/cpp                      │
│    - Global memory manager singleton                │
│    - Region management                              │
│    - Heap creation and tracking                     │
│    - Context stack for allocations                  │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│           Memory Regions                            │
│      Code/Sys/Mem/region.h/cpp                      │
│    - Physical memory partitions                     │
│    - Bottom-up and top-down allocators              │
│    - Memory availability tracking                   │
└──────────────────┬──────────────────────────────────┘
                   ↓
      ┌────────────┴──────────┬──────────────┐
      ↓                       ↓              ↓
┌───────────┐         ┌──────────┐    ┌──────────┐
│   Heap    │         │   Pool   │    │  Region  │
│ Allocator │         │Allocator │    │ Allocator│
└───────────┘         └──────────┘    └──────────┘
      ↓                       ↓              ↓
┌─────────────────────────────────────────────────┐
│         Application Objects & Data              │
└─────────────────────────────────────────────────┘
```

### Design Philosophy

1. **Explicit Control**: Manual memory management for predictable performance
2. **Region-Based**: Partition memory into regions with different allocators
3. **Pool Allocation**: Pre-allocate fixed-size objects for efficiency
4. **No Fragmentation**: Careful allocation strategies minimize fragmentation
5. **Debugging Support**: Built-in tracking and leak detection

---

## Memory Regions

### Region Class

**File**: `Code/Sys/Mem/region.h`

A `Region` represents a contiguous block of memory:

```cpp
class Region : public Spt::Class
{
public:
    void* StartAddr() const;        // Region start address
    void* EndAddr() const;          // Region end address
    
    void RegisterAllocator(Allocator* alloc);
    void UnregisterAllocator(Allocator* alloc);
    
    void* Allocate(Allocator* pAlloc, size_t size, 
                   bool assert_on_fail = true);
    
    int MemAvailable();             // Available memory
    int MinMemAvailable();          // Minimum ever available
    int TotalSize();                // Total region size
};
```

### Region Types

**AllocRegion**: Basic allocated region

```cpp
class AllocRegion : public Region
{
    AllocRegion(size_t size);       // Allocate from system
    virtual ~AllocRegion();         // Free to system
};
```

**StaticRegion**: Pre-existing memory block

```cpp
class StaticRegion : public Region
{
    StaticRegion(void* pStart, void* pEnd);  // Use existing memory
};
```

### Direction of Allocation

Regions support two allocation directions:

```cpp
enum Direction
{
    vBOTTOM_UP,     // Allocate from low addresses up
    vTOP_DOWN,      // Allocate from high addresses down
};
```

**Use Cases**:
- **Bottom-Up**: General game data, grows upward
- **Top-Down**: Temporary allocations, grows downward
- **Both**: Meet in the middle, maximize utilization

### Region Example

```cpp
// Create a 10MB region
AllocRegion* pRegion = new AllocRegion(10 * 1024 * 1024);

// Create bottom-up heap
Heap* pHeap = new Heap(pRegion, Mem::vBOTTOM_UP, "Main Heap");

// Create top-down pool
Pool* pPool = new Pool(pRegion, sizeof(MyObject), 100, 
                       Mem::vTOP_DOWN);

// Allocations now share the region efficiently
```

---

## Allocators

### Allocator Base Class

**File**: `Code/Sys/Mem/alloc.h`

All allocators derive from the `Allocator` base class:

```cpp
class Allocator
{
public:
    enum Direction
    {
        vBOTTOM_UP,
        vTOP_DOWN,
    };
    
    virtual void* allocate(size_t size, bool assert_on_fail) = 0;
    virtual void free(BlockHeader* pHeader) = 0;
    virtual int available() = 0;
    
    Direction GetDirection() const;
    Region* GetRegion() const;
};
```

### Block Header

Each allocation includes a header for tracking:

```cpp
struct BlockHeader
{
    size_t size;            // Allocation size (including header)
    Allocator* pAllocator;  // Allocator that created this block
    
    // Debug information (in debug builds)
    #ifdef __NOPT_ASSERT__
    const char* pFileName;  // Source file
    int lineNumber;         // Source line
    uint32 checksum;        // Name checksum (optional)
    #endif
};
```

---

## Memory Pools

### Pool Allocator

**File**: `Code/Sys/Mem/pool.h`

Pools pre-allocate fixed-size blocks for efficient allocation:

```cpp
class Pool : public Allocator
{
public:
    Pool(Region* region, size_t size, uint count, 
         Direction dir = vBOTTOM_UP);
    virtual ~Pool();
    
private:
    virtual void* allocate(size_t size, bool assert_on_fail);
    virtual void free(BlockHeader* pHeader);
    
    BlockHeader* mp_free_list;  // Free block linked list
    uint m_count;               // Total blocks
    size_t m_size;              // Block size
};
```

### Pool Properties

**Advantages**:
- **Fast Allocation**: O(1) allocation/deallocation
- **No Fragmentation**: Fixed-size blocks
- **Cache-Friendly**: Contiguous memory
- **Predictable**: Known memory footprint

**Trade-offs**:
- **Fixed Size**: Can't allocate different sizes
- **Pre-allocation**: Memory used even when blocks unused
- **Limited Count**: Must specify maximum blocks

### Pool Usage

```cpp
// Create pool for 1000 particles, each 64 bytes
Pool* pParticlePool = new Pool(pRegion, 64, 1000, 
                               Mem::vBOTTOM_UP);

// Allocate particle
Particle* pParticle = (Particle*)pParticlePool->Allocate(
    sizeof(Particle), true);

// Free particle
pParticlePool->Free(pParticle);
```

### Compact Pool

**File**: `Code/Sys/Mem/CompactPool.h`

Optimized pool for common use cases:

```cpp
class CCompactPool
{
public:
    CCompactPool(int itemSize, int maxItems, char* name = NULL);
    ~CCompactPool();
    
    void* Allocate();           // Get free item
    void Free(void* pItem);     // Return item
    
    int GetMaxUsedItems() const;
    int GetNumUsedItems() const;
    int GetTotalItems() const;
};
```

---

## Poolable Objects

### CPoolable Template

**File**: `Code/Sys/Mem/poolable.h`

Classes can be made poolable with the `CPoolable` template:

```cpp
template <class _T>
class CPoolable
{
public:
    static void SCreatePool(int num_items, char* name);
    static void SAttachPool(CCompactPool* pPool);
    static void SRemovePool();
    
    static int SGetMaxUsedItems();
    static int SGetNumUsedItems();
    static int SGetTotalItems();
    
    void* operator new(size_t size);
    void operator delete(void* pMem);
    
private:
    static CCompactPool* sp_pool[POOL_STACK_SIZE];
    static bool s_internallyCreatedPool[POOL_STACK_SIZE];
    static int s_currentPool;
};
```

### Defining Poolable Classes

**Usage**:

```cpp
// 1. Inherit from CPoolable<T>
class MyClass : public Mem::CPoolable<MyClass>
{
public:
    MyClass();
    ~MyClass();
    
    int m_data;
};

// 2. Define pool in source file
DefinePoolableClass(MyClass);

// 3. Create pool before using class
void Initialize()
{
    MyClass::SCreatePool(100, "MyClass Pool");
}

// 4. Use normal new/delete
MyClass* pObj = new MyClass();
delete pObj;  // Returns to pool, not OS
```

### Pool Stack

Pools support multiple contexts (pool stack):

```cpp
// Create first pool
MyClass::SCreatePool(100, "Main Pool");

// Switch to second pool
MyClass::SSwitchToNextPool();
MyClass::SCreatePool(50, "Secondary Pool");

// Allocations now use secondary pool
MyClass* pObj = new MyClass();

// Switch back to first pool
MyClass::SSwitchToPreviousPool();
```

---

## Memory Heaps

### Heap Allocator

**File**: `Code/Sys/Mem/heap.h`

Heaps provide variable-size allocation:

```cpp
class Heap : public Allocator
{
public:
    Heap(Region* region, Direction dir = vBOTTOM_UP, 
         char* p_name = "unknown heap");
    
    int LargestFreeBlock();
    
private:
    virtual void* allocate(size_t size, bool assert_on_fail);
    virtual void free(BlockHeader* pHeader);
    
    virtual int available();
    virtual void* reallocate_down(size_t new_size, void* pOld);
    virtual void* reallocate_up(size_t new_size, void* pOld);
    virtual void* reallocate_shrink(size_t new_size, void* pOld);
};
```

### Heap Types

The memory manager maintains multiple heaps:

```cpp
#define NUM_PERM_SKATER_HEAPS   1
#define NUM_SKATER_HEAPS        8
#define NUM_NAMED_HEAPS         4

#define DEBUG_HEAP_SIZE         32767*1024  // ~32MB
```

**Heap Categories**:

1. **Main Heap**: General game allocations
2. **Skater Heaps**: Per-skater data (up to 8 players)
3. **Named Heaps**: Custom purpose heaps
4. **Debug Heap**: Debug allocations (tracking, logging)

### Context Stack

**File**: `Code/Sys/Mem/memman.h`

The manager uses a context stack for allocations:

```cpp
class Manager : public Spt::Class
{
public:
    void PushContext(Allocator* pAlloc);
    void PopContext();
    Allocator* GetContext() const;
    
    void* New(size_t size, bool assert_on_fail = true, 
              Allocator* pAlloc = NULL);
    void Delete(void* pMem);
};
```

**Usage**:

```cpp
// Temporarily allocate from different heap
Mem::Manager::sHandle().PushContext(pCustomHeap);
void* pData = new char[1024];
Mem::Manager::sHandle().PopContext();

// pData allocated from pCustomHeap, not default context
```

---

## Memory Budgets

### Budget Tracking

Games have strict memory budgets per platform:

**Console Budgets** (approximate):
- PlayStation 2: 32 MB total RAM
- Xbox: 64 MB total RAM
- GameCube: 24 MB main + 16 MB ARAM

**PC Budgets**: Configurable, typically 128-256 MB

### Budget Allocation

Memory is allocated to subsystems:

```
Total Memory: 32 MB (PS2)
  ├── Code/Executables: 4 MB
  ├── Textures: 8 MB
  ├── Models/Geometry: 6 MB
  ├── Audio: 4 MB
  ├── Scripts: 2 MB
  ├── Animation: 3 MB
  ├── Game Objects: 3 MB
  └── System/Stack: 2 MB
```

### Memory Monitoring

```cpp
// Check available memory
int available = pRegion->MemAvailable();

// Check minimum available (fragmentation indicator)
int minAvailable = pRegion->MinMemAvailable();

// Check total size
int total = pRegion->TotalSize();

// Calculate usage
int used = total - available;
float usage = (float)used / (float)total * 100.0f;
```

### Low Memory Handling

When memory is low, the game can:

1. **Unload Assets**: Remove unused textures, models
2. **Defragment**: Compact heap to free contiguous space
3. **Reduce Quality**: Lower texture resolution, LOD
4. **Stream**: Load/unload data dynamically

---

## Debugging Tools

### Memory Tracking

**File**: `Code/Sys/Mem/memdbg.h`

Debug builds include extensive tracking:

```cpp
#ifdef __NOPT_ASSERT__
struct AllocationInfo
{
    const char* fileName;       // Allocation source file
    int lineNumber;             // Allocation source line
    uint32 checksum;            // Name checksum (optional)
    size_t size;                // Allocation size
    void* address;              // Memory address
};
#endif
```

### Memory Leak Detection

The system can detect leaks on shutdown:

```cpp
void Manager::CheckForLeaks()
{
    // Iterate all regions
    for (each region)
    {
        // Check for unreleased allocations
        if (region->MemAvailable() != region->TotalSize())
        {
            // Report leak
            printf("Leak in region: %d bytes unreleased\n",
                   region->TotalSize() - region->MemAvailable());
            
            // Dump allocation list (debug builds)
            #ifdef __NOPT_ASSERT__
            region->DumpAllocations();
            #endif
        }
    }
}
```

### Memory Visualization

Debug builds can visualize memory usage:

```
[Main Heap - 10 MB]
████████░░░░░░░░░░░░░░░░ 40% used (4 MB)
  
[Particle Pool - 64 KB]
██████████████████████░░ 90% used (576 items / 640)

[Texture Region - 8 MB]
████████████████████████ 100% used (8 MB)
```

### Memory Test

**File**: `Code/Sys/Mem/memtest.h`

Utilities for testing memory system:

```cpp
class MemTest
{
public:
    // Test allocation/deallocation
    static void TestHeap();
    static void TestPool();
    
    // Stress test
    static void StressTest(int iterations);
    
    // Fragmentation test
    static void FragmentationTest();
};
```

---

## Best Practices

### 1. Use Pools for Frequent Allocations

```cpp
// BAD: Frequent heap allocations
void Update()
{
    Particle* p = new Particle();  // Heap allocation
    // ... use particle ...
    delete p;                      // Heap deallocation
}

// GOOD: Use pool
DefinePoolableClass(Particle);

void Initialize()
{
    Particle::SCreatePool(1000, "Particle Pool");
}

void Update()
{
    Particle* p = new Particle();  // Pool allocation (fast!)
    // ... use particle ...
    delete p;                      // Pool deallocation (fast!)
}
```

### 2. Minimize Allocations in Performance-Critical Code

```cpp
// BAD: Allocate every frame
void Update()
{
    char* buffer = new char[1024];
    // ... use buffer ...
    delete[] buffer;
}

// GOOD: Allocate once, reuse
class MySystem
{
    char m_buffer[1024];  // Static buffer
    
    void Update()
    {
        // Use m_buffer, no allocation
    }
};
```

### 3. Group Related Allocations

```cpp
// BAD: Scattered allocations
Texture* pTex1 = LoadTexture("tex1.img");
Model* pModel = LoadModel("model.mdl");
Texture* pTex2 = LoadTexture("tex2.img");
// Memory fragmented

// GOOD: Group by type
Texture* pTex1 = LoadTexture("tex1.img");
Texture* pTex2 = LoadTexture("tex2.img");
Model* pModel = LoadModel("model.mdl");
// Textures contiguous, better cache coherency
```

### 4. Use Context Stack for Temporary Allocations

```cpp
// Create temporary heap
Heap* pTempHeap = CreateTemporaryHeap();

// Push context
Mem::Manager::sHandle().PushContext(pTempHeap);

// All allocations use temp heap
char* pData = new char[1024];
int* pArray = new int[100];

// Pop context (back to default heap)
Mem::Manager::sHandle().PopContext();

// Destroy temp heap (all allocations freed at once!)
DestroyTemporaryHeap(pTempHeap);
```

### 5. Track Memory Usage

```cpp
void MonitorMemory()
{
    // Check main heap
    int available = pMainHeap->GetRegion()->MemAvailable();
    int total = pMainHeap->GetRegion()->TotalSize();
    
    float usage = (1.0f - (float)available / (float)total) * 100.0f;
    
    printf("Main Heap: %.1f%% used (%d / %d bytes)\n",
           usage, total - available, total);
    
    // Check pools
    printf("Particle Pool: %d / %d items\n",
           Particle::SGetNumUsedItems(),
           Particle::SGetTotalItems());
}
```

### 6. Free Resources in Reverse Order

```cpp
// Allocation order
pTextureA = LoadTexture("a.img");
pTextureB = LoadTexture("b.img");
pModel = LoadModel("model.mdl");

// Free in reverse order (LIFO)
UnloadModel(pModel);
UnloadTexture(pTextureB);
UnloadTexture(pTextureA);

// Reduces fragmentation if using bottom-up heap
```

---

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall engine architecture
- [docs/CODE_STYLE.md](../CODE_STYLE.md) - Coding conventions

## Implementation Files

### Core Memory System
- `Code/Sys/Mem/memman.h/cpp` - Memory manager
- `Code/Sys/Mem/region.h/cpp` - Memory regions
- `Code/Sys/Mem/alloc.h/cpp` - Allocator base class
- `Code/Sys/Mem/heap.h/cpp` - Heap allocator
- `Code/Sys/Mem/pool.h/cpp` - Pool allocator
- `Code/Sys/Mem/CompactPool.h/cpp` - Compact pool

### Poolable System
- `Code/Sys/Mem/Poolable.h/cpp` - Poolable template
- `Code/Sys/Mem/PoolManager.h/cpp` - Pool manager

### Utilities
- `Code/Sys/Mem/handle.h` - Memory handles
- `Code/Sys/Mem/memptr.h` - Memory pointers
- `Code/Sys/Mem/pile.h/cpp` - Memory pile (stack)

### Debugging
- `Code/Sys/Mem/memdbg.h` - Memory debugging
- `Code/Sys/Mem/memtest.h/cpp` - Memory testing

---

*This document describes the memory management system in the THUG engine.*
