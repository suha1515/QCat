project "QCat"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .."/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .."/%{prj.name}")

	disablewarnings{"4819"}
	characterset ("MBCS")

	pchheader "qcpch.h"
	pchsource "src/qcpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"3rdLib/stb_image/**.h",
		"3rdLib/stb_image/**.cpp",
		"3rdLib/glm/glm/**.hpp",
		"3rdLib/glm/glm/**.inl",
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"3rdLib/spdlog/include",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}"
	}

	links
	{
		"d3d11.lib",
		"opengl32.lib",
		"ImGui",
		"Glad",
		"yaml-cpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "QCAT_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "on"

