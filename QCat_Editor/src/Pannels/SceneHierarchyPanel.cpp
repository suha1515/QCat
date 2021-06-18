#include "SceneHierarchyPanel.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include <glm/gtc/type_ptr.hpp>
#include<QCat/Scene/Components.h>
#include <QCat/Uitiliy/PlatformUtils.h>

namespace QCat
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = Entity();
	}
	void SceneHierarchyPanel::OnImguiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		auto view = m_Context->m_Registry.view<RelationShipComponent>();

		for (auto entity : view)
		{
			RelationShipComponent& comp = view.get<RelationShipComponent>(entity);
			if (comp.parent == Entity::emptyEntity)
			{
				Entity ent = { entity,m_Context.get() };
				DrawEntityNode(ent);
			}
		}
		/*m_Context->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID,m_Context.get() }; 
			DrawEntityNode(entity);	
		});*/
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = Entity();

		// Right-Click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectionContext!=Entity::emptyEntity)
		{
			DrawComponents(m_SelectionContext);
		}
			
		ImGui::End();
	}
	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity("Empty Entity", &entity);
			}
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

				ImGui::EndPopup();
		}
		if (opened)
		{
			RelationShipComponent& comp = entity.GetComponent<RelationShipComponent>();
			auto curr = comp.firstChild;
			while (curr != Entity::emptyEntity)
			{
				auto nextSiling = curr.GetComponent<RelationShipComponent>().nextSibling;
				DrawEntityNode(curr);
				curr = nextSiling;
			}
			ImGui::TreePop();
		}
		if (entityDeleted)
		{
			if (m_SelectionContext == entity)
				m_SelectionContext = Entity();
			m_Context->DestroyEntity(entity);
		}
	}
	static void DrawVec3Control(const std::string& label,glm::vec3& values ,float resetValue = 0.0f,float columnwidth =100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		//bold Font (initialized in imguilayer)
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnwidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f,0.1f,0.15f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f,0.2f,0.2f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f,0.1f,0.15f,1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f,0.0f,0.0f,"%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f,0.7f,0.2f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f,0.8f,0.3f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f,0.7f,0.2f,1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f,0.35f,0.9f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
	template<typename T,typename UIFunction>
	static void DrawComponent(const std::string& name,Entity entity,UIFunction uifunction)
	{
		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed|ImGuiTreeNodeFlags_FramePadding| ImGuiTreeNodeFlags_SpanAvailWidth|ImGuiTreeNodeFlags_AllowItemOverlap;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight,lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
				ImGui::EndPopup();
			}
			if (open)
			{
				uifunction(component);
				ImGui::TreePop();
			}
			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}
	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				if (!m_SelectionContext.HasComponent<CameraComponent>())
					m_SelectionContext.AddComponent<CameraComponent>();
				else
					QCAT_CORE_WARN("This Entity already has Camera Component");
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("SpriteRenderer"))
			{
				if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
				else
					QCAT_CORE_WARN("This Entity already has SpriteRendererComponent");

				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("MeshComponent"))
			{
				if (!m_SelectionContext.HasComponent<MeshComponent>())
					m_SelectionContext.AddComponent<MeshComponent>();
				else
					QCAT_CORE_WARN("This Entity already has MeshComponent");
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("MaterialComponent"))
			{
				if (!m_SelectionContext.HasComponent<MaterialComponent>())
					m_SelectionContext.AddComponent<MaterialComponent>();
				else
					QCAT_CORE_WARN("This Entity already has MaterialComponent");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			}
		);
		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Prespective","Orthographic" };
				const char* currentProjectionStrings = projectionTypeStrings[static_cast<int>(camera.GetProjectionType())];
				if (ImGui::BeginCombo("Projection", currentProjectionStrings))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentProjectionStrings == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionStrings = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float fovSize = glm::degrees(camera.GetPerspectiveVerticalFov());
					if (ImGui::DragFloat("VerticalFov", &fovSize))
						camera.SetPerspectiveVerticalFov(glm::radians(fovSize));

					float perspectivNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectivNear))
						camera.SetPerspectiveNearClip(perspectivNear);

					float perspectivFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectivFar))
						camera.SetPerspectiveNearClip(perspectivFar);
				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthoGraphicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthoGraphicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed AspectRatio", &component.FixedAspectRatio);
				}
			}
		);

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, 
			[](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			}
		);
		DrawComponent<MeshComponent>("Mesh", entity,
			[](auto& component)
			{
				static ImGuiComboFlags flags = 0;
				MeshComponent& mesh = component;
				const auto& vertexArray = mesh.GetMeshes();
				std::vector<std::string> meshNames;
				for (const auto& mesh : MeshLibrary::GetMeshes())
				{
					meshNames.push_back(mesh.second->GetMeshName());
				}
				static int item_current_idx = 0;
				std::string comboLabel = meshNames[item_current_idx];
				if (ImGui::BeginCombo("SavedMeshList", comboLabel.c_str(), flags))
				{
					for (int i = 0; i < meshNames.size(); ++i)
					{
						const bool is_selected = (item_current_idx == i);
						if (ImGui::Selectable(meshNames[i].c_str(), is_selected))
							item_current_idx = i;

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					std::string currentSelectedMeshName = meshNames[item_current_idx];
					auto iter = std::find_if(mesh.vertexArray.begin(), mesh.vertexArray.end(),
						[&](const Ref<VertexArray>& mesh)
						{
							return mesh->GetMeshName() == currentSelectedMeshName;
						});
					if(iter == mesh.vertexArray.end())
						mesh.vertexArray.push_back(MeshLibrary::Load(currentSelectedMeshName));
					else
					{

					}
				}
				
				ImGui::ListBoxHeader("MeshList");
				static int mesh_current_idx = 0;
				for (int i=0;i<vertexArray.size();++i)
				{
					const bool is_selected = (mesh_current_idx == i);
					std::string meshName = vertexArray[i]->GetMeshName();
					if (ImGui::Selectable(meshName.c_str(), is_selected))
						mesh_current_idx = i;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();

			}
			);
		DrawComponent<MaterialComponent>("Material", entity,
			[](auto& component)
			{
				Material& mat = component.GetMaterial();

				ImGui::ColorEdit3("Ambient", glm::value_ptr(mat.ambient));
				ImGui::ColorEdit3("Diffuse", glm::value_ptr(mat.diffuse));
				ImGui::ColorEdit3("Specaulr", glm::value_ptr(mat.specular));

				ImGui::DragFloat("Shininess",&mat.shininess);
				ImGui::DragFloat("Metalic", &mat.metallic);
				ImGui::DragFloat("Rougness",&mat.roughness);
				ImGui::DragFloat("Ao", &mat.ao);

				ImGui::Separator();
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnOffset(0, 21);
				ImGui::Separator();
				if (mat.m_DiffuseTexture)
					ImGui::Image(mat.m_DiffuseTexture->GetTexture(), ImVec2(64, 64), RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? ImVec2{ 0,1 }, ImVec2{ 1,0 } : ImVec2{ 0,0 }, ImVec2{ 1,1 });
				ImGui::NextColumn();
				ImGui::Text("Albedo Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##albedo"))
				{
					OpenTexture(mat.m_DiffuseTexture);
				}
				if (mat.m_DiffuseTexture)
					ImGui::Text(mat.m_DiffuseTexture->path.c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_NormalMapTexture)
					ImGui::Image(mat.m_NormalMapTexture->GetTexture(), ImVec2(64, 64), RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? ImVec2{ 0,1 }, ImVec2{ 1,0 } : ImVec2{ 0,0 }, ImVec2{ 1,1 });
				ImGui::NextColumn();
				ImGui::Text("Normal Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##normal"))
				{
					 OpenTexture(mat.m_NormalMapTexture);
				}
				if (mat.m_NormalMapTexture)
					ImGui::Text(mat.m_NormalMapTexture->path.c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_MetallicTexture)
					ImGui::Image(mat.m_MetallicTexture->GetTexture(), ImVec2(64, 64), RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? ImVec2{ 0,1 }, ImVec2{ 1,0 } : ImVec2{ 0,0 }, ImVec2{ 1,1 });
				ImGui::NextColumn();
				ImGui::Text("Metallic Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##metaillic"))
				{
					OpenTexture(mat.m_MetallicTexture);
				}
				if (mat.m_MetallicTexture)
					ImGui::Text(mat.m_MetallicTexture->path.c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_RoughnessTexture)
					ImGui::Image(mat.m_RoughnessTexture->GetTexture(), ImVec2(64, 64), RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? ImVec2{ 0,1 }, ImVec2{ 1,0 } : ImVec2{ 0,0 }, ImVec2{ 1,1 });
				ImGui::NextColumn();
				ImGui::Text("Roughness Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##roughness"))
				{
					OpenTexture(mat.m_RoughnessTexture );
				}
				if (mat.m_RoughnessTexture)
					ImGui::Text(mat.m_RoughnessTexture->path.c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_AmbientOcclusionTexture)
					ImGui::Image(mat.m_AmbientOcclusionTexture->GetTexture(), ImVec2(64, 64), RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? ImVec2{ 0,1 }, ImVec2{ 1,0 } : ImVec2{ 0,0 }, ImVec2{ 1,1 });
				ImGui::NextColumn();
				ImGui::Text("AmbientOcclusion Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##ambient"))
				{
					OpenTexture(mat.m_AmbientOcclusionTexture);
				}
				if (mat.m_AmbientOcclusionTexture)
					ImGui::Text(mat.m_AmbientOcclusionTexture->path.c_str());
				else
					ImGui::Text("none");

				ImGui::Columns(1);
				ImGui::Separator();
				
			}
			);
	}
	void OpenTexture(Ref<Texture>& texture)
	{
		Sampler_Desc desc;
		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;
		std::optional<std::string> filepath = FileDialogs::OpenFile("Image File \0*.PNG;.JPEG\0");
		if (filepath)
		{
			texture = TextureLibrary::Load(*filepath, desc, 1, 1, RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? false : true);
		}
	}
	void ShowMeshList()
	{
		std::string chooseMesh = "None";
		ImGui::Begin("MeshList");
		auto& meshes = MeshLibrary::GetMeshes();
		for (auto& mesh : meshes)
		{
			ImGui::Button(mesh.second->GetMeshName().c_str());
		}
		ImGui::End();
	}
}