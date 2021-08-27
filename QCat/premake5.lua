project "QCat"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

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
		"%{IncludeDir.assimp}",
		"%{IncludeDir.VulkanSDK}"
	}

	links
	{
		"d3d11.lib",
		"opengl32.lib",
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
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
			"%{Library.Assimp_Debug}"
		}
	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "on"
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "on"
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
