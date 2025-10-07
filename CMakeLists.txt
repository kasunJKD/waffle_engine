cmake_minimum_required(VERSION 3.23)
project(Sandbox LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Build type & output folders
# ---------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected – defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug / Release" FORCE)
endif()

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
foreach(dir RUNTIME LIBRARY ARCHIVE)
    set(CMAKE_${dir}_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
endforeach()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Compiler warnings & optimisation flags
# ---------------------------------------------------------------------------
if (MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O2)
    endif()
endif()

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendors")

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

set(IMGUI_SRC
    "${VENDOR_DIR}/imgui/imgui.cpp"
    "${VENDOR_DIR}/imgui/imgui_draw.cpp"
    "${VENDOR_DIR}/imgui/imgui_tables.cpp"
    "${VENDOR_DIR}/imgui/imgui_widgets.cpp"
    "${VENDOR_DIR}/imgui/imgui_demo.cpp"       # remove if not needed
    "${VENDOR_DIR}/imgui/backends/imgui_impl_sdl2.cpp"
    "${VENDOR_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)

set(GLAD_SRC "${VENDOR_DIR}/include/glad/src/glad.c")

add_executable(Sandbox
    ${PROJECT_SRC}
    ${IMGUI_SRC}
    ${GLAD_SRC}
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Sandbox PRIVATE DEBUG_ENABLED)
endif()

# ---------------------------------------------------------------------------
# Include dirs
# ---------------------------------------------------------------------------
target_include_directories(Sandbox PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${VENDOR_DIR}"
    "${VENDOR_DIR}/include"
    "${VENDOR_DIR}/include/glad/include"
    "${VENDOR_DIR}/include/SDL2"
    "${VENDOR_DIR}/include/freetype"
    "${VENDOR_DIR}/include/assimp"
    "${VENDOR_DIR}/imgui"
    "${VENDOR_DIR}/imgui/backends"
)

target_compile_definitions(Sandbox PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# ---------------------------------------------------------------------------
# Windows-specific libraries
# ---------------------------------------------------------------------------
if (WIN32)
    find_package(OpenGL REQUIRED)

    # Directories where .lib files live
    target_link_directories(Sandbox PRIVATE
        "${VENDOR_DIR}/lib/SDL2"
        "${VENDOR_DIR}/lib/assimp"
        "${VENDOR_DIR}/lib/openal"
        "${VENDOR_DIR}/lib/freetype"
    )

    target_link_libraries(Sandbox PRIVATE
        SDL2 SDL2main
        OpenAL32
        assimp-vc143-mt
        freetype
        ${OPENGL_gl_LIBRARY}   # opengl32 on Win
        imm32                  # ImGui needs it, otherwise unresolved externals
        setupapi               # imgui_impl_win32 uses it on multi-window setups
    )

    # Nice-to-have: makes F5 in VS run from your repo root
    if (MSVC)
        set_target_properties(Sandbox PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    endif()
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(Sandbox PRIVATE -Wno-language-extension-token)
endif()
