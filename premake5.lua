workspace("SandboxSolution")
configurations({ "Debug", "Release" })
platforms({ "x64" })
location("build")

project("Sandbox")
kind("ConsoleApp")
language("C++")
cppdialect("C++17")
targetdir("bin/%{cfg.buildcfg}")
objdir("bin-int/%{cfg.buildcfg}")

files({
	"src/**.cpp",
	"src/**.h",
	"vendors/include/glad/src/glad.c",
	"vendors/imgui/*.cpp",
	"vendors/imgui/backends/imgui_impl_sdl2.cpp",
	"vendors/imgui/backends/imgui_impl_opengl3.cpp",
})

includedirs({
	"src",
	"vendors/include",
	"vendors/include/glad/include",
	"vendors/include/SDL2",
	"vendors/include/freetype",
	"vendors/include/assimp",
	"vendors/imgui",
	"vendors/imgui/backends",
})

libdirs({
	"vendors/lib/SDL2",
	"vendors/lib/assimp",
	"vendors/lib/openal",
	"vendors/lib/freetype",
})

links({
	"SDL2",
	"SDL2main",
	"OpenAL32",
	"assimp-vc143-mt",
	"freetype",
	"opengl32",
	"imm32",
	"setupapi",
})

filter("system:windows")
systemversion("latest")
buildoptions("/W4")

filter("configurations:Debug")
runtime("Debug")
symbols("On")
defines({ "DEBUG_ENABLED", "IMGUI_IMPL_OPENGL_LOADER_GLAD" })

filter("configurations:Release")
runtime("Release")
optimize("Speed")
defines({ "NDEBUG", "IMGUI_IMPL_OPENGL_LOADER_GLAD" })
