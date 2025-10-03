cmake_minimum_required(VERSION 3.10)
project(THUG VERSION 1.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

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

# Collect source files
# We don't follow symlinks to avoid duplicates from case-insensitive symlinks
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_SOURCE_DIR}/Code/Core/*.cpp"
)

file(GLOB_RECURSE GEL_SOURCES 
    "${CMAKE_SOURCE_DIR}/Code/Gel/*.cpp"
)

file(GLOB_RECURSE GFX_SOURCES 
    "${CMAKE_SOURCE_DIR}/Code/Gfx/*.cpp"
)

file(GLOB_RECURSE SK_SOURCES 
    "${CMAKE_SOURCE_DIR}/Code/Sk/*.cpp"
)

file(GLOB_RECURSE SYS_SOURCES 
    "${CMAKE_SOURCE_DIR}/Code/Sys/*.cpp"
)

# Exclude platform-specific files
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/ngps/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/ngc/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/NGPS/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/NGC/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/xbox/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/XBox/.*")

list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/ngps/.*")
list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/ngc/.*")
list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/NGPS/.*")
list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/NGC/.*")
list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/xbox/.*")
list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/XBox/.*")

list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/ngps/.*")
list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/ngc/.*")
list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/NGPS/.*")
list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/NGC/.*")
list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/xbox/.*")
list(FILTER GFX_SOURCES EXCLUDE REGEX ".*/XBox/.*")

list(FILTER SK_SOURCES EXCLUDE REGEX ".*/ngps/.*")
list(FILTER SK_SOURCES EXCLUDE REGEX ".*/ngc/.*")
list(FILTER SK_SOURCES EXCLUDE REGEX ".*/Ngps/.*")
list(FILTER SK_SOURCES EXCLUDE REGEX ".*/NGC/.*")
list(FILTER SK_SOURCES EXCLUDE REGEX ".*/Ngc/.*")

list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/ngps/.*")
list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/ngc/.*")
list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/NGPS/.*")
list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/NGC/.*")
list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/xbox/.*")
list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/XBox/.*")

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GEL_SOURCES}
    ${GFX_SOURCES}
    ${SK_SOURCES}
    ${SYS_SOURCES}
)

# Debug: Print source count
list(LENGTH ALL_SOURCES SOURCE_COUNT)
message(STATUS "Found ${SOURCE_COUNT} source files")
if(SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "No source files found! Check the Code directory structure.")
endif()

# Create executable
add_executable(thug ${ALL_SOURCES})

# Link libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(thug pthread dl m)
elseif(APPLE)
    target_link_libraries(thug pthread dl m)
endif()

# Installation
install(TARGETS thug DESTINATION bin)

# Print build information
message(STATUS "Building THUG for ${PLATFORM_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
