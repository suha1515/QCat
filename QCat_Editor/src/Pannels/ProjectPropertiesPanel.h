#pragma once
#include <Imgui/imgui.h>

namespace QCat
{
	class ProjectPropertiesPanel
	{
	public:
		ProjectPropertiesPanel();

		void OnImGuiRender();

	public:
		// Shadow properties
		bool m_SoftShadow = true;
		float m_ShadowMaxDistance = 100.0f;
		// Cascade properties
		bool m_IsDebugCascade = false;
		float m_CascadeSplitsLamda = 0.5f;
	};
}