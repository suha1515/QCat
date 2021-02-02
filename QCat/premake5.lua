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

	libdirs { "%{wks.location}/QCat/3rdLib/assimp/Debug" }

	files
	{
		"src/**.h",
		"src/**.cpp",
		"3rdLib/stb_image/**.h",
		"3rdLib/stb_image/**.cpp",
		"3rdLib/glm/glm/**.hpp",
		"3rdLib/glm/glm/**.inl",
		"3rdLib/ImGuizmo/ImGuizmo.h",
		"3rdLib/ImGuizmo/ImGuizmo.cpp"
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
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.assimp}"
	}

	links
	{
		"d3d11.lib",
		"opengl32.lib",
		"assimp-vc142-mtd.lib",
		"ImGui",
		"Glad",
		"yaml-cpp"
	}
	filter "files:3rdLib/ImGuizmo/**.cpp"
	flags {"NoPCH"}

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

