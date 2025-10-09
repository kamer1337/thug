# Sys subsystem CMakeLists.txt
# System-level and platform-specific code

# Collect Sys source files
file(GLOB_RECURSE SYS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

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
