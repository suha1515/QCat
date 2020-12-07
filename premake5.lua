workspace "QCat"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architexture}"

-- Include directories
IncludeDir = {}
IncludeDir["ImGui"] = "QCat/3rdLib/Imgui"
IncludeDir["DirectXMath"] = "QCat/3rdLib/DirectXMath"
IncludeDir["Glad"] = "QCat/3rdLib/Glad/Include"

include "QCat/3rdLib/Imgui"
include "QCat/3rdLib/Glad"

project "QCat"
	location "QCat"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

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
		"%{prj.name}/3rdLib/DirectXMath/**.h",
		"%{prj.name}/3rdLib/DirectXMath/**.cpp",
		"%{prj.name}/3rdLib/DirectXMath/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/3rdLib/spdlog/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.DirectXMath}",
		"%{IncludeDir.Glad}"
	}

	links
	{
		"d3d11.lib",
		"opengl32.lib",
		"ImGui",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"QCAT_PLATFORM_WINDOWS",
			"QCAT_BUILD_LIB"
		}

	filter "configurations:Debug"
		defines "QCAT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "QCAT_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		runtime "Release"
		optimize "on"

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

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
		"QCat/src",
		"%{IncludeDir.DirectXMath}"
	}

	links
	{
		"QCat"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"QCAT_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "QCAT_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "on"


