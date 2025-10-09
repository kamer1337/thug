# Gel subsystem CMakeLists.txt
# Game Engine Layer - high-level game systems

# Collect Gel source files
file(GLOB_RECURSE GEL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

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
