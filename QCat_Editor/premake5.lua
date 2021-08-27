project "QCat_Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .."/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .."/%{prj.name}")

	disablewarnings{"4819"}
	characterset ("MBCS")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/QCat/3rdLib/spdlog/include",
		"%{wks.location}/QCat/src",
		"%{wks.location}/QCat/3rdLib",
		"%{IncludeDir.Glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.assimp}"
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
		postbuildcommands
		{
			"{COPY} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\""
		}
		postbuildcommands
		{
			"{COPY} \"%{LibraryDir.Assimp_DebugDLL}\" \"%{cfg.targetdir}\""
		}
		
	filter "configurations:Release"
		defines "QCAT_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "QCAT_DIST"
		optimize "on"

