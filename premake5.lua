require("../export-compile-commands")

workspace("MyGame")
configurations({ "Debug", "Release" })
platforms({ "x64" })
location("build")

toolset("clang")

project("MyGame")
kind("ConsoleApp")
language("C++")
targetdir("bin/%{cfg.buildcfg}")

includedirs({
	"vendors/include",
	"vendors/include/glad/include",
	"vendors/include/SDL2",
	"vendors",
	"imgui",
	"imgui/backends",
	"vendors/include/assimp",
})

files({
	"src/main.cpp",
	"src/shader.cpp",
	"src/mesh.cpp",
	"src/model.h",
	"src/textureManager.h",
	"vendors/include/glad/src/glad.c",
	"imgui/imgui*.cpp",
	"imgui/backends/imgui_impl_sdl2.cpp",
	"imgui/backends/imgui_impl_opengl3.cpp",
	"src/camera.cpp",
	"src/grid.h",
})

filter("system:windows")
links({ "user32", "opengl32", "SDL2", "SDL2main", "assimp-vc143-mt" })
libdirs({ "vendors/lib/SDL2", "vendors/lib/assimp" })

filter("system:linux")
links({ "GL", "dl", "pthread", "SDL2", "assimp" })

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")
