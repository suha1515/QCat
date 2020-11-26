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

	pchheader "qcpch.h"
	pchsource "QCat/src/qcpch.cpp"

	disablewarnings{"4819"}
	characterset ("MBCS")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/3rdLib/spdlog/include",
	}

	links
	{
		"d3d11.lib",
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
	characterset ("MBCS")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/resource.h",
		"%{prj.name}/SandBox.rc",
		"%{prj.name}/SandBox.aps",
		"%{prj.name}/Resource/**.ico"
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


