cmake_minimum_required(VERSION 3.10)
project(THUG VERSION 1.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# ============================================================================
# Backend Architecture Configuration
# ============================================================================

# Graphics Backend Options
option(USE_VULKAN_RENDERER "Use Vulkan renderer backend (recommended)" OFF)
option(USE_DIRECTX_RENDERER "Use DirectX renderer backend (Windows only)" OFF)
option(USE_OPENGL_RENDERER "Use OpenGL renderer backend" OFF)

# Audio Backend Selection
set(AUDIO_BACKEND "None" CACHE STRING "Audio backend to use (DirectSound/SDL2/OpenAL/FMOD/None)")
set_property(CACHE AUDIO_BACKEND PROPERTY STRINGS DirectSound SDL2 OpenAL FMOD None)

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

# ============================================================================
# Graphics Backend Configuration
# ============================================================================

# Validate graphics backend selection
set(GRAPHICS_BACKEND_COUNT 0)
if(USE_VULKAN_RENDERER)
    math(EXPR GRAPHICS_BACKEND_COUNT "${GRAPHICS_BACKEND_COUNT} + 1")
endif()
if(USE_DIRECTX_RENDERER)
    math(EXPR GRAPHICS_BACKEND_COUNT "${GRAPHICS_BACKEND_COUNT} + 1")
endif()
if(USE_OPENGL_RENDERER)
    math(EXPR GRAPHICS_BACKEND_COUNT "${GRAPHICS_BACKEND_COUNT} + 1")
endif()

# Ensure only one graphics backend is selected
if(GRAPHICS_BACKEND_COUNT GREATER 1)
    message(FATAL_ERROR "Multiple graphics backends selected. Please select only one of: USE_VULKAN_RENDERER, USE_DIRECTX_RENDERER, USE_OPENGL_RENDERER")
endif()

# Platform-specific backend validation
if(USE_DIRECTX_RENDERER AND NOT WIN32)
    message(FATAL_ERROR "DirectX renderer is only supported on Windows")
endif()

# Vulkan renderer configuration
if(USE_VULKAN_RENDERER)
    message(STATUS "Graphics Backend: Vulkan (recommended)")
    add_definitions(-DUSE_VULKAN_RENDERER)
    
    # Try to find Vulkan SDK (optional, for future full implementation)
    find_package(Vulkan QUIET)
    if(Vulkan_FOUND)
        message(STATUS "  Vulkan SDK found: ${Vulkan_LIBRARY}")
        include_directories(${Vulkan_INCLUDE_DIRS})
    else()
        message(STATUS "  Vulkan SDK not found (optional for current implementation)")
    endif()
endif()

# DirectX renderer configuration
if(USE_DIRECTX_RENDERER)
    message(STATUS "Graphics Backend: DirectX (Windows only)")
    add_definitions(-DUSE_DIRECTX_RENDERER)
    message(STATUS "  DirectX implementation is a stub - requires DirectX SDK integration")
endif()

# OpenGL renderer configuration
if(USE_OPENGL_RENDERER)
    message(STATUS "Graphics Backend: OpenGL (cross-platform)")
    add_definitions(-DUSE_OPENGL_RENDERER)
    
    # Try to find OpenGL
    find_package(OpenGL QUIET)
    if(OPENGL_FOUND)
        message(STATUS "  OpenGL found: ${OPENGL_LIBRARIES}")
        include_directories(${OPENGL_INCLUDE_DIR})
    else()
        message(STATUS "  OpenGL not found (optional for current implementation)")
    endif()
endif()

# Default message if no graphics backend is selected
if(GRAPHICS_BACKEND_COUNT EQUAL 0)
    message(STATUS "Graphics Backend: None selected (using platform default)")
    message(STATUS "  Available options:")
    message(STATUS "    -DUSE_VULKAN_RENDERER=ON   (recommended, cross-platform)")
    if(WIN32)
        message(STATUS "    -DUSE_DIRECTX_RENDERER=ON  (Windows only)")
    endif()
    message(STATUS "    -DUSE_OPENGL_RENDERER=ON   (cross-platform)")
endif()

# ============================================================================
# Audio Backend Configuration
# ============================================================================

# Validate and configure audio backend
if(NOT AUDIO_BACKEND STREQUAL "None")
    message(STATUS "Audio Backend: ${AUDIO_BACKEND}")
    
    if(AUDIO_BACKEND STREQUAL "DirectSound")
        add_definitions(-DUSE_DIRECTSOUND_AUDIO)
        message(STATUS "  DirectSound implementation is a stub - requires DirectSound SDK (Windows only)")
        if(NOT WIN32)
            message(WARNING "  DirectSound is only available on Windows. Consider using SDL2 or OpenAL for cross-platform audio.")
        endif()
        message(STATUS "  Note: DirectSound is deprecated by Microsoft - consider modern alternatives")
        
    elseif(AUDIO_BACKEND STREQUAL "SDL2")
        add_definitions(-DUSE_SDL2_AUDIO)
        message(STATUS "  SDL2_mixer implementation is a stub - requires SDL2_mixer integration")
        message(STATUS "  Install: sudo apt install libsdl2-mixer-dev (Linux)")
        message(STATUS "           brew install sdl2_mixer (macOS)")
        
    elseif(AUDIO_BACKEND STREQUAL "OpenAL")
        add_definitions(-DUSE_OPENAL_AUDIO)
        message(STATUS "  OpenAL implementation is a stub - requires OpenAL integration")
        message(STATUS "  Install: sudo apt install libopenal-dev (Linux)")
        message(STATUS "           brew install openal-soft (macOS)")
        
    elseif(AUDIO_BACKEND STREQUAL "FMOD")
        add_definitions(-DUSE_FMOD_AUDIO)
        message(STATUS "  FMOD implementation is a stub - requires FMOD SDK")
        message(STATUS "  Download: https://www.fmod.com/download")
        message(STATUS "  Note: FMOD requires a commercial license for released games")
        
    else()
        message(FATAL_ERROR "Invalid AUDIO_BACKEND: ${AUDIO_BACKEND}. Valid options: DirectSound, SDL2, OpenAL, FMOD, None")
    endif()
else()
    message(STATUS "Audio Backend: None selected (using platform default stubs)")
    message(STATUS "  Available options:")
    message(STATUS "    -DAUDIO_BACKEND=DirectSound (legacy Windows API, deprecated)")
    message(STATUS "    -DAUDIO_BACKEND=SDL2        (recommended, simple API, free)")
    message(STATUS "    -DAUDIO_BACKEND=OpenAL      (3D positional audio, free)")
    message(STATUS "    -DAUDIO_BACKEND=FMOD        (professional features, requires license)")
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

# Link graphics backend libraries
if(USE_VULKAN_RENDERER AND Vulkan_FOUND)
    target_link_libraries(thug ${Vulkan_LIBRARIES})
    message(STATUS "Linking Vulkan library: ${Vulkan_LIBRARIES}")
endif()

if(USE_OPENGL_RENDERER AND OPENGL_FOUND)
    target_link_libraries(thug ${OPENGL_LIBRARIES})
    message(STATUS "Linking OpenGL library: ${OPENGL_LIBRARIES}")
endif()

if(USE_DIRECTX_RENDERER)
    if(WIN32)
        # DirectX libraries (will be linked when implementation is complete)
        # target_link_libraries(thug d3d9 d3dx9)
        message(STATUS "DirectX linking will be enabled when implementation is complete")
    endif()
endif()

# Link audio backend libraries (when implementations are complete)
if(AUDIO_BACKEND STREQUAL "SDL2")
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        find_path(SDL2_MIXER_INCLUDE_DIR SDL2/SDL_mixer.h)
        find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
        
        if(SDL2_MIXER_INCLUDE_DIR AND SDL2_MIXER_LIBRARY)
            target_include_directories(thug PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIR})
            target_link_libraries(thug ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARY})
            message(STATUS "SDL2_mixer: Found and linked")
        else()
            message(STATUS "SDL2_mixer: Not found - audio will use stub implementation")
            message(STATUS "  Install: sudo apt install libsdl2-mixer-dev (Linux)")
            message(STATUS "           brew install sdl2_mixer (macOS)")
        endif()
    else()
        message(STATUS "SDL2: Not found - audio will use stub implementation")
    endif()
    
elseif(AUDIO_BACKEND STREQUAL "OpenAL")
    find_package(OpenAL QUIET)
    if(OPENAL_FOUND)
        target_include_directories(thug PRIVATE ${OPENAL_INCLUDE_DIR})
        target_link_libraries(thug ${OPENAL_LIBRARY})
        message(STATUS "OpenAL: Found and linked - ${OPENAL_LIBRARY}")
    else()
        message(STATUS "OpenAL: Not found - audio will use stub implementation")
        message(STATUS "  Install: sudo apt install libopenal-dev (Linux)")
        message(STATUS "           brew install openal-soft (macOS)")
    endif()
    
elseif(AUDIO_BACKEND STREQUAL "FMOD")
    # FMOD requires manual SDK download and installation
    set(FMOD_ROOT "" CACHE PATH "Path to FMOD SDK root directory")
    
    if(FMOD_ROOT)
        if(WIN32)
            set(FMOD_LIB_PATH "${FMOD_ROOT}/api/core/lib/x64")
            set(FMOD_INCLUDE_PATH "${FMOD_ROOT}/api/core/inc")
        elseif(APPLE)
            set(FMOD_LIB_PATH "${FMOD_ROOT}/api/core/lib")
            set(FMOD_INCLUDE_PATH "${FMOD_ROOT}/api/core/inc")
        else()
            set(FMOD_LIB_PATH "${FMOD_ROOT}/api/core/lib/x86_64")
            set(FMOD_INCLUDE_PATH "${FMOD_ROOT}/api/core/inc")
        endif()
        
        find_library(FMOD_LIBRARY fmod PATHS ${FMOD_LIB_PATH})
        
        if(FMOD_LIBRARY AND EXISTS ${FMOD_INCLUDE_PATH})
            target_include_directories(thug PRIVATE ${FMOD_INCLUDE_PATH})
            target_link_libraries(thug ${FMOD_LIBRARY})
            message(STATUS "FMOD: Found and linked - ${FMOD_LIBRARY}")
        else()
            message(STATUS "FMOD: Library not found in ${FMOD_ROOT}")
        endif()
    else()
        message(STATUS "FMOD: SDK path not set (use -DFMOD_ROOT=/path/to/fmod)")
        message(STATUS "  Download: https://www.fmod.com/download")
        message(STATUS "  Note: FMOD requires a commercial license for released games")
    endif()
endif()

# Video backend configuration
option(USE_FFMPEG_VIDEO "Use FFmpeg for video playback" OFF)

if(USE_FFMPEG_VIDEO)
    # Find FFmpeg components
    find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
    find_library(AVCODEC_LIBRARY avcodec)
    find_library(AVFORMAT_LIBRARY avformat)
    find_library(AVUTIL_LIBRARY avutil)
    find_library(SWSCALE_LIBRARY swscale)
    find_library(SWRESAMPLE_LIBRARY swresample)
    
    if(AVCODEC_INCLUDE_DIR AND AVCODEC_LIBRARY AND AVFORMAT_LIBRARY AND AVUTIL_LIBRARY AND SWSCALE_LIBRARY)
        add_definitions(-DUSE_FFMPEG_VIDEO)
        target_include_directories(thug PRIVATE ${AVCODEC_INCLUDE_DIR})
        target_link_libraries(thug 
            ${AVCODEC_LIBRARY}
            ${AVFORMAT_LIBRARY}
            ${AVUTIL_LIBRARY}
            ${SWSCALE_LIBRARY}
            ${SWRESAMPLE_LIBRARY}
        )
        message(STATUS "FFmpeg: Found and linked for video playback")
        message(STATUS "  libavcodec: ${AVCODEC_LIBRARY}")
        message(STATUS "  libavformat: ${AVFORMAT_LIBRARY}")
        message(STATUS "  libavutil: ${AVUTIL_LIBRARY}")
        message(STATUS "  libswscale: ${SWSCALE_LIBRARY}")
    else()
        message(STATUS "FFmpeg: Not found - video playback will use stub implementation")
        message(STATUS "  Install: sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev (Linux)")
        message(STATUS "           brew install ffmpeg (macOS)")
    endif()
else()
    message(STATUS "FFmpeg: Disabled (use -DUSE_FFMPEG_VIDEO=ON to enable)")
endif()

# Installation
install(TARGETS thug DESTINATION bin)

# ============================================================================
# Build Summary
# ============================================================================
message(STATUS "")
message(STATUS "=================================================================")
message(STATUS "THUG Build Configuration Summary")
message(STATUS "=================================================================")
message(STATUS "Platform:        ${PLATFORM_NAME}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler:    ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler:      ${CMAKE_C_COMPILER}")
message(STATUS "")
if(USE_VULKAN_RENDERER)
    message(STATUS "Graphics:        Vulkan (recommended)")
elseif(USE_DIRECTX_RENDERER)
    message(STATUS "Graphics:        DirectX (Windows)")
elseif(USE_OPENGL_RENDERER)
    message(STATUS "Graphics:        OpenGL (cross-platform)")
else()
    message(STATUS "Graphics:        Platform default (no backend selected)")
endif()
if(NOT AUDIO_BACKEND STREQUAL "None")
    message(STATUS "Audio:           ${AUDIO_BACKEND}")
else()
    message(STATUS "Audio:           Platform default (no backend selected)")
endif()
message(STATUS "=================================================================")
message(STATUS "")
