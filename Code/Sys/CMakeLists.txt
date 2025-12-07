# Sys subsystem CMakeLists.txt
# System-level and platform-specific code

# Collect Sys source files
file(GLOB_RECURSE SYS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Exclude platform-specific files based on the platform
if(NOT WIN32)
    # Exclude Win32/Wn32 specific files on non-Windows platforms
    list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/Win32/.*")
    list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/Wn32/.*")
endif()

# Exclude SDL2 files unless SDL2 window is explicitly enabled
if(NOT USE_SDL2_WINDOW)
    list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/SDL2/.*")
endif()

# Exclude console-specific SIO files on PC platforms
# (PC uses Win32/p_siodev.cpp and Win32/p_keyboard.cpp instead)
if(UNIX OR WIN32)
    list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/SIO/siodev\\.cpp$")
    list(FILTER SYS_SOURCES EXCLUDE REGEX ".*/SIO/sioman\\.cpp$")
endif()

# Create Sys library
add_library(thug_sys OBJECT ${SYS_SOURCES})

# Sys include directories
target_include_directories(thug_sys PUBLIC
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Sys depends on Core
target_link_libraries(thug_sys PUBLIC thug_core)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(thug_sys PRIVATE __PLAT_WN32__)
endif()

# Debug configurations
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(thug_sys PRIVATE
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
endif()

# Export sources for parent build
set(SYS_SOURCES ${SYS_SOURCES} PARENT_SCOPE)

message(STATUS "Sys subsystem configured with ${CMAKE_CURRENT_SOURCE_DIR}")
