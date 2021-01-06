workspace "QCat"
	architecture "x64"
	startproject "SandBox"
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
IncludeDir["Glm"] = "QCat/3rdLib/glm"
IncludeDir["Glad"] = "QCat/3rdLib/Glad/Include"
IncludeDir["stb_image"] = "QCat/3rdLib/stb_image"

group "Dependencies"
	include "QCat/3rdLib/Glad"
	include "QCat/3rdLib/ImGui"
group""

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
		"%{prj.name}/3rdLib/stb_image/**.h",
		"%{prj.name}/3rdLib/stb_image/**.cpp",
		"%{prj.name}/3rdLib/DirectXMath/**.h",
		"%{prj.name}/3rdLib/DirectXMath/**.cpp",
		"%{prj.name}/3rdLib/DirectXMath/**.inl",
		"%{prj.name}/3rdLib/glm/glm/**.hpp",
		"%{prj.name}/3rdLib/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/3rdLib/spdlog/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.DirectXMath}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.stb_image}"
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
		"%{prj.name}/src/**.hlsl",
		"%{prj.name}/resource.h",
		"%{prj.name}/SandBox.rc",
		"%{prj.name}/SandBox.aps",
		"%{prj.name}/Resource/**.ico"
	}

	includedirs
	{
		"QCat/3rdLib/spdlog/include",
		"QCat/src",
		"%{IncludeDir.DirectXMath}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.ImGui}"
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

	
project "QCat_Editor"
	location "QCat_Editor"
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
	}

	includedirs
	{
		"QCat/3rdLib/spdlog/include",
		"QCat/src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.ImGui}"
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

