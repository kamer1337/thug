# Sk subsystem CMakeLists.txt
# Skateboarding game logic and mechanics

# Collect Sk source files
file(GLOB_RECURSE SK_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Create Sk library
add_library(thug_sk OBJECT ${SK_SOURCES})

# Sk include directories
target_include_directories(thug_sk PUBLIC
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Sk depends on Core and Gel
target_link_libraries(thug_sk PUBLIC thug_core thug_gel)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(thug_sk PRIVATE __PLAT_WN32__)
endif()

# Debug configurations
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(thug_sk PRIVATE
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
endif()

# Export sources for parent build
set(SK_SOURCES ${SK_SOURCES} PARENT_SCOPE)

message(STATUS "Sk subsystem configured with ${CMAKE_CURRENT_SOURCE_DIR}")
