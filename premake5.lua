workspace("Engine")
configurations({ "Debug", "Release" })
platforms({ "x64" })
location("build")

project("Engine")
kind("StaticLib") -- Static library for Engine
language("C++")
targetdir("bin/%{cfg.buildcfg}/Engine")
includedirs({
	"vendors/include",
	"vendors/include/glad/include", -- Include glad headers
	"vendors/include/SDL2",
	"vendors",
	"imgui",
	"imgui/backends",
	"vendors/include/assimp",
})
files({
	"engine/**.cpp",
	"engine/**.h",
	"vendors/include/glad/src/glad.c", -- Ensure glad.c is included in the project
})
filter("system:windows")
links({ "SDL2", "SDL2main", "OpenAL32", "assimp-vc143-mt" })
libdirs({ "vendors/lib/SDL2", "vendors/lib/assimp", "vendors/lib/openal", "vendors/lib/assimp/assimp-vc143-mt.lib" })

project("Sandbox")
kind("ConsoleApp")
language("C++")
targetdir("bin/%{cfg.buildcfg}/Sandbox")
files({
	"sandbox/**.cpp",
	"sandbox/**.h",
})
includedirs({
	"engine",
	"sandbox",
	"vendors",
	"vendors/include", -- Include SDL2, GLAD, and other dependencies
	"vendors/include/SDL2",
	"vendors/include/glad/include",
	"imgui",
	"imgui/backends",
	"vendors/include/assimp",
})
links({ "Engine", "SDL2", "SDL2main", "OpenAL32" }) -- Include glad here
libdirs({ "vendors/lib/SDL2", "vendors/lib/assimp", "vendors/lib/openal" })
