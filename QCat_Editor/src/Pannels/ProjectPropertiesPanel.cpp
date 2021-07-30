#include "ProjectPropertiesPanel.h"

namespace QCat
{
	ProjectPropertiesPanel::ProjectPropertiesPanel()
	{
	}
	void ProjectPropertiesPanel::OnImGuiRender()
	{
		ImGui::Begin("Project property");

		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
		bool cascade_open = ImGui::TreeNodeEx("Cascade Settings", flags);
		if (cascade_open)
		{
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100.0f);
			//Soft Shadow
			ImGui::Text("Soft Shadow");
			ImGui::NextColumn();
			ImGui::Checkbox("##SoftShadow", &m_SoftShadow);
			ImGui::NextColumn();
			// Shadow MaxDistance
			ImGui::Text("Shadow MaxDistance");
			ImGui::NextColumn();
			ImGui::DragFloat("##ShadowMaxDistance", &m_ShadowMaxDistance,1.0f, 0.0f, 400.0f);
			ImGui::NextColumn();
			//Debug Cascade Map
			ImGui::Text("Show Cascade");
			ImGui::NextColumn();
			//Debug Cascade Map
			ImGui::Checkbox("##DebugCascadeShadow", &m_IsDebugCascade);
			ImGui::NextColumn();

			// Splits Lamda
			ImGui::Text("Cascade Splits");
			ImGui::NextColumn();
			ImGui::DragFloat("##CascadeSplits", &m_CascadeSplitsLamda, 0.01f, 0.0f, 1.0f);
			ImGui::TreePop();
			}
		ImGui::End();
	}
}