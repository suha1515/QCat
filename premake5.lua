include "./3rdLib/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"
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

group "Dependencies"
	include "3rdLib/premake"
	include "QCat/3rdLib/Glad"
	include "QCat/3rdLib/ImGui"
	include "QCat/3rdLib/yaml-cpp"

group""

include "QCat"
include "SandBox"
include "QCat_Editor"
