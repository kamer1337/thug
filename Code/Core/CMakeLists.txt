# Core subsystem CMakeLists.txt
# Foundational utilities, data structures, and platform abstractions

# Collect Core source files
file(GLOB_RECURSE CORE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Exclude platform-specific files based on the platform
if(NOT WIN32)
    # Exclude Win32/Wn32 specific files on non-Windows platforms
    list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/Win32/.*")
    list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/Wn32/.*")
endif()

# Create Core library
add_library(thug_core OBJECT ${CORE_SOURCES})

# Core include directories
target_include_directories(thug_core PUBLIC
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform-specific settings
if(WIN32)
    # Core uses some Win32 APIs
    target_compile_definitions(thug_core PRIVATE __PLAT_WN32__)
endif()

# Debug configurations
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(thug_core PRIVATE
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
endif()

# Export sources for parent build
set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)

message(STATUS "Core subsystem configured with ${CMAKE_CURRENT_SOURCE_DIR}")
