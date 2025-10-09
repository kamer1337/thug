# Gfx subsystem CMakeLists.txt
# Graphics rendering subsystem

# Collect Gfx source files
file(GLOB_RECURSE GFX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Create Gfx library
add_library(thug_gfx OBJECT ${GFX_SOURCES})

# Gfx include directories
target_include_directories(thug_gfx PUBLIC
    ${CMAKE_SOURCE_DIR}/Code
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Gfx depends on Core
target_link_libraries(thug_gfx PUBLIC thug_core)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(thug_gfx PRIVATE __PLAT_WN32__)
endif()

# Graphics backend configuration
if(USE_VULKAN_RENDERER)
    target_compile_definitions(thug_gfx PRIVATE USE_VULKAN_RENDERER)
    if(Vulkan_FOUND)
        target_include_directories(thug_gfx PRIVATE ${Vulkan_INCLUDE_DIRS})
    endif()
    message(STATUS "  Gfx: Vulkan renderer enabled")
endif()

if(USE_DIRECTX_RENDERER)
    target_compile_definitions(thug_gfx PRIVATE USE_DIRECTX_RENDERER)
    message(STATUS "  Gfx: DirectX renderer enabled")
endif()

if(USE_OPENGL_RENDERER)
    target_compile_definitions(thug_gfx PRIVATE USE_OPENGL_RENDERER)
    if(OPENGL_FOUND)
        target_include_directories(thug_gfx PRIVATE ${OPENGL_INCLUDE_DIR})
    endif()
    message(STATUS "  Gfx: OpenGL renderer enabled")
endif()

# Debug configurations
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(thug_gfx PRIVATE
        __NOPT_DEBUG__
        __NOPT_ASSERT__
        __NOPT_MESSAGES__
    )
endif()

# Export sources for parent build
set(GFX_SOURCES ${GFX_SOURCES} PARENT_SCOPE)

message(STATUS "Gfx subsystem configured with ${CMAKE_CURRENT_SOURCE_DIR}")
