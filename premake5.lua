workspace "QCat"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architexture}"

project "QCat"
	location "QCat"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	disablewarnings{"4819"}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/3rdLib/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"QCAT_PLATFORM_WINDOWS",
			"QCAT_BUILD_LIB"
		}

	filter "configurations:Debug"
		defines "QCAT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "On"

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"

	language "C++"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	disablewarnings{"4819"}
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"QCat/3rdLib/spdlog/include",
		"QCat/src"
	}

	links
	{
		"QCat"
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"QCAT_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "QCAT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "On"

