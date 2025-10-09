cmake_minimum_required(VERSION 3.10)
project(THUG VERSION 1.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# Renderer options
option(USE_VULKAN_RENDERER "Use Vulkan renderer backend (experimental)" OFF)

# Compiler options
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wno-unknown-pragmas -Wno-format)
endif()

# Platform detection
if(WIN32)
    set(PLATFORM_NAME "Win32")
    add_definitions(-D__PLAT_WN32__)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_NAME "Linux")
    add_definitions(-D__PLAT_LINUX__)
elseif(APPLE)
    set(PLATFORM_NAME "MacOS")
    add_definitions(-D__PLAT_MACOS__)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Debug flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D__NOPT_DEBUG__ -D__NOPT_ASSERT__ -D__NOPT_MESSAGES__)
endif()

# Create case-insensitive include wrapper
# The original code was written for Windows (case-insensitive filesystem)
# We need to handle this on Linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-write-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated -Wno-write-strings")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Code
)

# Vulkan renderer configuration
if(USE_VULKAN_RENDERER)
    message(STATUS "Vulkan renderer enabled (experimental)")
    add_definitions(-DUSE_VULKAN_RENDERER)
    
    # Try to find Vulkan SDK (optional, for future full implementation)
    find_package(Vulkan QUIET)
    if(Vulkan_FOUND)
        message(STATUS "Vulkan SDK found: ${Vulkan_LIBRARY}")
        include_directories(${Vulkan_INCLUDE_DIRS})
    else()
        message(STATUS "Vulkan SDK not found (optional for current implementation)")
    endif()
endif()

# Add subsystem directories (modular CMake structure)
# Each subsystem has its own CMakeLists.txt
add_subdirectory(Code/Core)
add_subdirectory(Code/Gel)
add_subdirectory(Code/Gfx)
add_subdirectory(Code/Sk)
add_subdirectory(Code/Sys)

# Combine all subsystem sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GEL_SOURCES}
    ${GFX_SOURCES}
    ${SK_SOURCES}
    ${SYS_SOURCES}
)

# Debug: Print source count
list(LENGTH ALL_SOURCES SOURCE_COUNT)
message(STATUS "Found ${SOURCE_COUNT} source files from modular subsystems")
if(SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "No source files found! Check the Code directory structure.")
endif()

# Create executable using object libraries from subsystems
add_executable(thug 
    $<TARGET_OBJECTS:thug_core>
    $<TARGET_OBJECTS:thug_gel>
    $<TARGET_OBJECTS:thug_gfx>
    $<TARGET_OBJECTS:thug_sk>
    $<TARGET_OBJECTS:thug_sys>
)

# Link libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(thug pthread dl m)
elseif(APPLE)
    target_link_libraries(thug pthread dl m)
endif()

# Link Vulkan if available and enabled
if(USE_VULKAN_RENDERER AND Vulkan_FOUND)
    target_link_libraries(thug ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan library linked: ${Vulkan_LIBRARIES}")
endif()

# Installation
install(TARGETS thug DESTINATION bin)

# Print build information
message(STATUS "Building THUG for ${PLATFORM_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
