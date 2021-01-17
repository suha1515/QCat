include "./3rdLib/premake/premake_customization/solution_items.lua"

workspace "QCat"
	architecture "x64"
	startproject "QCat_Editor"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
		solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architexture}"

-- Include directories
IncludeDir = {}
IncludeDir["ImGui"] = "%{wks.location}/QCat/3rdLib/Imgui"
IncludeDir["Glm"] = "%{wks.location}/QCat/3rdLib/glm"
IncludeDir["Glad"] = "%{wks.location}/QCat/3rdLib/Glad/Include"
IncludeDir["stb_image"] = "%{wks.location}/QCat/3rdLib/stb_image"
IncludeDir["entt"] = "%{wks.location}/QCat/3rdLib/entt/Include"
IncludeDir["yaml_cpp"] = "%{wks.location}/QCat/3rdLib/yaml-cpp/Include"
IncludeDir["ImGuizmo"] = "%{wks.location}/QCat/3rdLib/ImGuizmo"


group "Dependencies"
	include "3rdLib/premake"
	include "QCat/3rdLib/Glad"
	include "QCat/3rdLib/ImGui"
	include "QCat/3rdLib/yaml-cpp"

group""

include "QCat"
include "SandBox"
include "QCat_Editor"
