# Gel subsystem CMakeLists.txt
# Game Engine Layer - high-level game systems

# Collect Gel source files
file(GLOB_RECURSE GEL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Exclude platform-specific files based on the platform
if(NOT WIN32)
    # Exclude Win32/Wn32 specific files on non-Windows platforms
    list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/Win32/.*")
    list(FILTER GEL_SOURCES EXCLUDE REGEX ".*/Wn32/.*")
endif()

# Create Gel library
add_library(thug_gel OBJECT ${GEL_SOURCES})

# Gel include directories
target_include_directories(thug_gel PUBLIC
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Gel depends on Core
target_link_libraries(thug_gel PUBLIC thug_core)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(thug_gel PRIVATE __PLAT_WN32__)
endif()

# Audio backend configuration
if(AUDIO_BACKEND STREQUAL "SDL2")
    target_compile_definitions(thug_gel PRIVATE USE_SDL2_AUDIO)
    message(STATUS "  Gel: SDL2_mixer audio backend enabled")
elseif(AUDIO_BACKEND STREQUAL "OpenAL")
    target_compile_definitions(thug_gel PRIVATE USE_OPENAL_AUDIO)
    message(STATUS "  Gel: OpenAL audio backend enabled")
elseif(AUDIO_BACKEND STREQUAL "FMOD")
    target_compile_definitions(thug_gel PRIVATE USE_FMOD_AUDIO)
    message(STATUS "  Gel: FMOD audio backend enabled")
endif()

# Debug configurations
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(thug_gel PRIVATE
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
endif()

# Export sources for parent build
set(GEL_SOURCES ${GEL_SOURCES} PARENT_SCOPE)

message(STATUS "Gel subsystem configured with ${CMAKE_CURRENT_SOURCE_DIR}")
