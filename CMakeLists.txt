cmake_minimum_required(VERSION 3.10)
project(THUG VERSION 1.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# Include platform-specific compilation flags
include(cmake/platform_flags.cmake)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Code
)

# Vulkan renderer configuration
if(USE_VULKAN_RENDERER)
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

# Link libraries (using THUG_PLATFORM_LIBS from platform_flags.cmake)
if(THUG_PLATFORM_LIBS)
    target_link_libraries(thug ${THUG_PLATFORM_LIBS})
endif()

# Link Vulkan if available and enabled
if(USE_VULKAN_RENDERER AND Vulkan_FOUND)
    target_link_libraries(thug ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan library linked: ${Vulkan_LIBRARIES}")
endif()

# Installation
install(TARGETS thug DESTINATION bin)

# =============================================================================
# Documentation Build Target (TASK-BUILD-006)
# =============================================================================

# Find Doxygen (optional)
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    # Doxygen configuration
    set(DOXYGEN_PROJECT_NAME "THUG Source Code")
    set(DOXYGEN_PROJECT_BRIEF "Tony Hawk's Underground - Source Code Documentation")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    set(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/Code ${CMAKE_SOURCE_DIR}/docs")
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_HTML_OUTPUT "html")
    set(DOXYGEN_FILE_PATTERNS "*.cpp *.h *.c *.md")
    set(DOXYGEN_EXCLUDE_PATTERNS "*/build/* */.git/*")
    
    # Create docs target
    doxygen_add_docs(
        docs
        ${CMAKE_SOURCE_DIR}/Code
        ${CMAKE_SOURCE_DIR}/docs
        COMMENT "Generating documentation with Doxygen"
    )
    
    message(STATUS "Documentation target 'docs' available (run 'make docs' or 'cmake --build . --target docs')")
else()
    message(STATUS "Doxygen not found - documentation target not available")
    message(STATUS "Install Doxygen to enable: apt-get install doxygen (Linux) or brew install doxygen (macOS)")
endif()

# Markdown documentation target (always available)
add_custom_target(markdown_docs
    COMMAND ${CMAKE_COMMAND} -E echo "=== THUG Documentation ==="
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Main Documentation Files:"
    COMMAND ${CMAKE_COMMAND} -E echo "  README.md - Project overview"
    COMMAND ${CMAKE_COMMAND} -E echo "  BUILDING.md - Build instructions"
    COMMAND ${CMAKE_COMMAND} -E echo "  ROADMAP.md - Development roadmap"
    COMMAND ${CMAKE_COMMAND} -E echo "  TASKS.md - Available tasks"
    COMMAND ${CMAKE_COMMAND} -E echo "  LEGAL.md - Legal information"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Architecture Documentation:"
    COMMAND ${CMAKE_COMMAND} -E echo "  docs/ARCHITECTURE.md"
    COMMAND ${CMAKE_COMMAND} -E echo "  docs/GAME_LOOP.md"
    COMMAND ${CMAKE_COMMAND} -E echo "  docs/RENDERING.md"
    COMMAND ${CMAKE_COMMAND} -E echo "  docs/SCRIPTING.md"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "All documentation available in the docs/ directory"
    COMMENT "Markdown documentation index"
)

message(STATUS "Markdown documentation target 'markdown_docs' available")

# =============================================================================
# Build Information
# =============================================================================

# =============================================================================
# Build Information
# =============================================================================

# Print build information (now handled by platform_flags.cmake summary)
