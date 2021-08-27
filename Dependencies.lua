
-- QCat Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/QCat/3rdLib/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/QCat/3rdLib/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/QCat/3rdLib/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/QCat/3rdLib/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/QCat/3rdLib/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/QCat/3rdLib/ImGuizmo"
IncludeDir["Glm"] = "%{wks.location}/QCat/3rdLib/glm"
IncludeDir["entt"] = "%{wks.location}/QCat/3rdLib/entt/include"
IncludeDir["assimp"] = "%{wks.location}/QCat/3rdLib/assimp/include"

IncludeDir["shaderc"] = "%{wks.location}/QCat/3rdLib/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/QCat/3rdLib/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/QCat/3rdLib/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/QCat/3rdLib/VulkanSDK/Bin"
LibraryDir["Assimp_Debug"] = "%{wks.location}/QCat/3rdLib/assimp/Lib"
LibraryDir["Assimp_DebugDLL"] = "%{wks.location}/QCat/3rdLib/assimp/DLL"

Library = {}
Library["Assimp_Debug"] = "%{LibraryDir.Assimp_Debug}/assimp-vc142-mtd.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
