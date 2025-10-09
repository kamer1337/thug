# Platform-specific compilation flags for THUG
# This module defines platform detection macros and sets appropriate compilation flags

# =============================================================================
# Platform Detection
# =============================================================================

message(STATUS "Detecting platform and setting compilation flags...")

# Detect operating system
if(WIN32)
    set(THUG_PLATFORM "Windows")
    set(THUG_PLATFORM_WINDOWS TRUE)
    add_compile_definitions(__PLAT_WN32__)
elseif(UNIX AND NOT APPLE)
    set(THUG_PLATFORM "Linux")
    set(THUG_PLATFORM_LINUX TRUE)
    add_compile_definitions(__PLAT_LINUX__)
elseif(APPLE)
    set(THUG_PLATFORM "macOS")
    set(THUG_PLATFORM_MACOS TRUE)
    add_compile_definitions(__PLAT_MACOS__)
else()
    message(WARNING "Unknown platform detected")
    set(THUG_PLATFORM "Unknown")
endif()

# Detect architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(THUG_ARCH "x64")
    set(THUG_ARCH_64BIT TRUE)
    add_compile_definitions(__ARCH_X64__)
else()
    set(THUG_ARCH "x86")
    set(THUG_ARCH_32BIT TRUE)
    add_compile_definitions(__ARCH_X86__)
endif()

message(STATUS "Platform: ${THUG_PLATFORM} (${THUG_ARCH})")

# =============================================================================
# Build Type Configuration
# =============================================================================

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Debug build flags
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
    message(STATUS "Debug flags enabled: assertions, debug messages")
endif()

# Release build optimizations
if(CMAKE_BUILD_TYPE MATCHES "Release")
    add_compile_definitions(__NOPT_FINAL__)
    message(STATUS "Release flags enabled: optimizations, final build")
endif()

# =============================================================================
# Compiler-Specific Flags
# =============================================================================

if(MSVC)
    # Microsoft Visual C++ flags
    message(STATUS "Compiler: MSVC")
    
    # Warning level
    add_compile_options(/W3)
    
    # Disable specific warnings
    add_compile_options(
        /wd4244  # conversion from 'type1' to 'type2', possible loss of data
        /wd4267  # conversion from 'size_t' to 'type', possible loss of data
        /wd4996  # deprecated function
    )
    
    # Enable multi-processor compilation
    add_compile_options(/MP)
    
    # Character set
    add_compile_definitions(_UNICODE UNICODE)
    
    # Debug flags
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(/O2)
    endif()
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC flags
    message(STATUS "Compiler: GCC ${CMAKE_CXX_COMPILER_VERSION}")
    
    # Warning flags
    add_compile_options(
        -Wall
        -Wno-unknown-pragmas
        -Wno-format
        -Wno-deprecated
        -Wno-write-strings
    )
    
    # Debug flags
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
    
    # Additional flags for compatibility
    add_compile_options(-fpermissive)
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang flags
    message(STATUS "Compiler: Clang ${CMAKE_CXX_COMPILER_VERSION}")
    
    # Warning flags
    add_compile_options(
        -Wall
        -Wno-unknown-pragmas
        -Wno-format
        -Wno-deprecated
        -Wno-write-strings
    )
    
    # Debug flags
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
    
    # Additional flags for compatibility
    add_compile_options(-fpermissive)
    
else()
    message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# =============================================================================
# Optional Features
# =============================================================================

# Vulkan renderer support
option(USE_VULKAN_RENDERER "Enable Vulkan renderer backend (experimental)" OFF)
if(USE_VULKAN_RENDERER)
    message(STATUS "Optional feature: Vulkan renderer enabled")
    add_compile_definitions(USE_VULKAN_RENDERER)
    add_compile_definitions(VULKAN_AVAILABLE)
endif()

# Enable profiling
option(ENABLE_PROFILING "Enable performance profiling" OFF)
if(ENABLE_PROFILING)
    message(STATUS "Optional feature: Profiling enabled")
    add_compile_definitions(__PROFILE__)
    if(NOT MSVC)
        add_compile_options(-pg)
        add_link_options(-pg)
    endif()
endif()

# Enable memory debugging
option(ENABLE_MEMORY_DEBUG "Enable memory allocation debugging" OFF)
if(ENABLE_MEMORY_DEBUG)
    message(STATUS "Optional feature: Memory debugging enabled")
    add_compile_definitions(__MEM_DEBUG__)
endif()

# Enable script debugging
option(ENABLE_SCRIPT_DEBUG "Enable script system debugging" OFF)
if(ENABLE_SCRIPT_DEBUG)
    message(STATUS "Optional feature: Script debugging enabled")
    add_compile_definitions(__SCRIPT_DEBUG__)
endif()

# =============================================================================
# Platform-Specific Libraries
# =============================================================================

# Set platform libraries variable
if(THUG_PLATFORM_WINDOWS)
    # Windows libraries
    set(THUG_PLATFORM_LIBS
        # Add Windows-specific libraries here
        # e.g., user32, gdi32, etc.
    )
elseif(THUG_PLATFORM_LINUX)
    # Linux libraries
    set(THUG_PLATFORM_LIBS
        pthread
        dl
        m
    )
elseif(THUG_PLATFORM_MACOS)
    # macOS libraries
    set(THUG_PLATFORM_LIBS
        pthread
        dl
        m
    )
endif()

# =============================================================================
# Summary
# =============================================================================

message(STATUS "=== Platform Configuration Summary ===")
message(STATUS "  Platform: ${THUG_PLATFORM}")
message(STATUS "  Architecture: ${THUG_ARCH}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "=====================================")
