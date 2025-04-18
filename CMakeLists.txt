cmake_minimum_required(VERSION 3.15)
project(EngineSolution)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Configure build types and output directories
# ---------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, defaulting to Debug")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

# For a 64-bit build (when supported by the generator)
if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
  if(NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM x64)
  endif()
endif()

# Set output directories for all targets
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# ---------------------------------------------------------------------------
# Compiler Flags
# ---------------------------------------------------------------------------
if(MSVC)
  # For MSVC, set warning level to /W4.
  add_compile_options(/W4)
else()
  # For Clang/GCC: Add warnings and extra flags.
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Werror -DDEBUG_ENABLED)
  else()
    add_compile_options(-O2)
  endif()
endif()

add_subdirectory(libc_w)

# ---------------------------------------------------------------------------
# Engine Static Library
# ---------------------------------------------------------------------------

# Collect source files for Engine.
# Note: Using file(GLOB_RECURSE ...) is convenient but not recommended for
# production since new files added to the directories are not automatically
# picked up by CMake unless you rerun CMake.
file(GLOB_RECURSE ENGINE_SOURCES
  "${CMAKE_SOURCE_DIR}/engine/*.cpp"
  "${CMAKE_SOURCE_DIR}/engine/*.h"
  "${CMAKE_SOURCE_DIR}/vendors/include/glad/src/glad.c"

"${CMAKE_SOURCE_DIR}/vendors/imgui/imgui.cpp"
"${CMAKE_SOURCE_DIR}/vendors/imgui/imgui_draw.cpp"
"${CMAKE_SOURCE_DIR}/vendors/imgui/imgui_tables.cpp"
"${CMAKE_SOURCE_DIR}/vendors/imgui/imgui_widgets.cpp"
"${CMAKE_SOURCE_DIR}/vendors/imgui/imgui_demo.cpp" # Optional
"${CMAKE_SOURCE_DIR}/vendors/imgui/backends/imgui_impl_sdl2.cpp"
"${CMAKE_SOURCE_DIR}/vendors/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(Engine STATIC ${ENGINE_SOURCES})

# Include directories for Engine.
target_include_directories(Engine PUBLIC
  "${CMAKE_SOURCE_DIR}/vendors/include"             # Common vendor headers
  "${CMAKE_SOURCE_DIR}/vendors/include/glad/include"  # GLAD headers
  "${CMAKE_SOURCE_DIR}/vendors/include/SDL2"          # SDL2 headers
  "${CMAKE_SOURCE_DIR}/vendors/include/freetype"          # SDL2 headers
  "${CMAKE_SOURCE_DIR}/vendors"                       # Additional vendor sources
  "${CMAKE_SOURCE_DIR}/vendors/imgui"                         # ImGui headers
  "${CMAKE_SOURCE_DIR}/imgui/backends"                # ImGui backend headers
  "${CMAKE_SOURCE_DIR}/vendors/include/assimp"        # Assimp headers
  "${CMAKE_SOURCE_DIR}/libc_w/include"
)

# Windows-specific linking for Engine.
if(WIN32)
  # Specify directories where the linker can find the libraries.
  target_link_directories(Engine PRIVATE
    "${CMAKE_SOURCE_DIR}/vendors/lib/SDL2"
    "${CMAKE_SOURCE_DIR}/vendors/lib/assimp"
    "${CMAKE_SOURCE_DIR}/vendors/lib/openal"
    "${CMAKE_SOURCE_DIR}/vendors/lib/freetype"
  )

  # Link against SDL2, SDL2main, OpenAL32, and the Assimp library.
  target_link_libraries(Engine PRIVATE
    SDL2
    SDL2main
    OpenAL32
    assimp-vc143-mt
    freetype
    libc_w
  )
endif()

# ---------------------------------------------------------------------------
# Sandbox Executable
# ---------------------------------------------------------------------------

# Collect source files for Sandbox.
file(GLOB_RECURSE SANDBOX_SOURCES
  "${CMAKE_SOURCE_DIR}/sandbox/*.cpp"
  "${CMAKE_SOURCE_DIR}/sandbox/*.h"
)

add_executable(Sandbox ${SANDBOX_SOURCES})

# Include directories for Sandbox.
target_include_directories(Sandbox PRIVATE
  "${CMAKE_SOURCE_DIR}/engine"                       # Engine project headers
  "${CMAKE_SOURCE_DIR}/sandbox"                      # Sandbox specific headers
  "${CMAKE_SOURCE_DIR}/vendors"                      # Additional vendor sources
  "${CMAKE_SOURCE_DIR}/vendors/include"              # Common vendor headers
  "${CMAKE_SOURCE_DIR}/vendors/include/SDL2"         # SDL2 headers
  "${CMAKE_SOURCE_DIR}/vendors/include/freetype"         # SDL2 headers
  "${CMAKE_SOURCE_DIR}/vendors/include/glad/include" # GLAD headers
  "${CMAKE_SOURCE_DIR}/vendors/imgui"                        # ImGui headers
  "${CMAKE_SOURCE_DIR}/imgui/backends"               # ImGui backend headers
  "${CMAKE_SOURCE_DIR}/vendors/include/assimp"       # Assimp headers
)

# Link Sandbox against the Engine library.
target_link_libraries(Sandbox PRIVATE Engine)

if(WIN32)
  # Add library directories for Sandbox as well.
  target_link_directories(Sandbox PRIVATE
    "${CMAKE_SOURCE_DIR}/vendors/lib/SDL2"
    "${CMAKE_SOURCE_DIR}/vendors/lib/assimp"
    "${CMAKE_SOURCE_DIR}/vendors/lib/freetype"
    "${CMAKE_SOURCE_DIR}/vendors/lib/openal"
  )

  # Link additional libraries required by Sandbox.
  target_link_libraries(Sandbox PRIVATE
    SDL2
    SDL2main
    OpenAL32
    freetype
  )
endif()

if(MSVC)
  set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  )
endif()
