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
			if (m_Context->GetEntityById(comp.parent) == Entity::emptyEntity)
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
				auto entity = m_Context->CreateEntity("Empty Entity");
				if(m_SelectionContext!= Entity())
					entity.SetParent(&m_SelectionContext);
			}
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

				ImGui::EndPopup();
		}
		if (opened)
		{
			RelationShipComponent& comp = entity.GetComponent<RelationShipComponent>();
			auto currid = comp.firstChild;
			auto currentity = m_Context->GetEntityById(currid);
			while (currentity != Entity::emptyEntity)
			{
				auto nextSibling = currentity.GetComponent<RelationShipComponent>().nextSibling;
				DrawEntityNode(currentity);
				currentity = m_Context->GetEntityById(nextSibling);
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
		ImVec2 uv1 = ImVec2(0.0f, 1.0f);
		ImVec2 uv2 = ImVec2(1.0f, 0.0f);
		uv1 = ImVec2(0.0f, 0.0f);
		uv2 = ImVec2(1.0f, 1.0f);
		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			uv1 = ImVec2(0.0f, 0.0f);
			uv2 = ImVec2(1.0f, 1.0f);
		}
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
			if (ImGui::MenuItem("LightComponent"))
			{
				if (!m_SelectionContext.HasComponent<LightComponent>())
					m_SelectionContext.AddComponent<LightComponent>();
				else
					QCAT_WARN("This Entity already has MaterialComponent");
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [&](auto& component)
			{
				glm::vec3 exTranslation = component.Translation;
				glm::vec3 exRotation = component.Rotation;
				glm::vec3 exScale = component.Scale;

				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);

				if (exTranslation != component.Translation || exRotation != component.Rotation || exScale != component.Scale)
				{
					component.changed = true;
				}
			}
		);
		DrawComponent<CameraComponent>("Camera", entity, [&](auto& component)
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
			[&](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			}
		);
		DrawComponent<MeshComponent>("Mesh", entity,
			[&](auto& component)
			{
				static ImGuiComboFlags flags = 0;
				MeshComponent& mesh = component;
				auto& vertexArray = mesh.GetMeshes();
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
				if(ImGui::Button("Load"))
				{
					OpenModel();
				}
				static int mesh_current_idx = 0;
				if (ImGui::ListBoxHeader("MeshList", vertexArray.size()))
				{
					for (int i = 0; i < vertexArray.size(); ++i)
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
				
				if (vertexArray.size() > 0)
				{
					if (mesh_current_idx<vertexArray.size())
					{
						if (ImGui::Button("Remove"))
						{
							vertexArray.erase(vertexArray.begin() + mesh_current_idx);
						}
					}
				}
			}
			);
		DrawComponent<MaterialComponent>("Material", entity,
			[&](auto& component)
			{
				Ref<Texture> checkerboardimg;
					
				checkerboardimg=TextureLibrary::Load("CheckerBoard");
				Material& mat = component.GetMaterial();

				ImGui::ColorEdit3("Ambient", glm::value_ptr(mat.ambient));
				ImGui::ColorEdit3("Diffuse", glm::value_ptr(mat.diffuse));
				ImGui::ColorEdit3("Specaulr", glm::value_ptr(mat.specular));

				ImGui::DragFloat("Shininess",&mat.shininess);
				ImGui::DragFloat("Metalic", &mat.metallic, 0.1f);
				ImGui::DragFloat("Rougness",&mat.roughness, 0.1f);
				ImGui::DragFloat("Ao", &mat.ao, 0.1f);

				ImGui::Separator();
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnOffset(0, 21);
				ImGui::Separator();
				if (mat.m_DiffuseTexture)
					ImGui::Image(mat.m_DiffuseTexture->GetTexture(), ImVec2(64, 64), uv1, uv2);
				else
					ImGui::Image(checkerboardimg->GetTexture(), ImVec2(64, 64), uv1, uv2);
				ImGui::NextColumn();
				ImGui::Text("Albedo Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##albedo"))
				{
					OpenTexture(mat.m_DiffuseTexture);
				}
				if (mat.m_DiffuseTexture)
					ImGui::Text(mat.m_DiffuseTexture->pathes[0].c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_NormalMapTexture)
					ImGui::Image(mat.m_NormalMapTexture->GetTexture(), ImVec2(64, 64), uv1, uv2);
				else
					ImGui::Image(checkerboardimg->GetTexture(), ImVec2(64, 64), uv1, uv2);
				ImGui::NextColumn();
				ImGui::Text("Normal Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##normal"))
				{
					 OpenTexture(mat.m_NormalMapTexture);
				}
				if (mat.m_NormalMapTexture)
					ImGui::Text(mat.m_NormalMapTexture->pathes[0].c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_MetallicTexture)
					ImGui::Image(mat.m_MetallicTexture->GetTexture(), ImVec2(64, 64), uv1, uv2);
				else
					ImGui::Image(checkerboardimg->GetTexture(), ImVec2(64, 64), uv1, uv2);
				ImGui::NextColumn();
				ImGui::Text("Metallic Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##metaillic"))
				{
					OpenTexture(mat.m_MetallicTexture);
				}
				if (mat.m_MetallicTexture)
					ImGui::Text(mat.m_MetallicTexture->pathes[0].c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_RoughnessTexture)
					ImGui::Image(mat.m_RoughnessTexture->GetTexture(), ImVec2(64, 64), uv1, uv2);
				else
					ImGui::Image(checkerboardimg->GetTexture(), ImVec2(64, 64), uv1, uv2);
					
				ImGui::NextColumn();
				ImGui::Text("Roughness Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##roughness"))
				{
					OpenTexture(mat.m_RoughnessTexture );
				}
				if (mat.m_RoughnessTexture)
					ImGui::Text(mat.m_RoughnessTexture->pathes[0].c_str());
				else
					ImGui::Text("none");

				ImGui::NextColumn();
				ImGui::Separator();
				if (mat.m_AmbientOcclusionTexture)
					ImGui::Image(mat.m_AmbientOcclusionTexture->GetTexture(), ImVec2(64, 64), uv1, uv2);
				else
					ImGui::Image(checkerboardimg->GetTexture(), ImVec2(64, 64), uv1, uv2);
				ImGui::NextColumn();
				ImGui::Text("AmbientOcclusion Texture");
				ImGui::SameLine();
				if (ImGui::SmallButton("Set Texture##ambient"))
				{
					OpenTexture(mat.m_AmbientOcclusionTexture);
				}
				if (mat.m_AmbientOcclusionTexture)
					ImGui::Text(mat.m_AmbientOcclusionTexture->pathes[0].c_str());
				else
					ImGui::Text("none");

				ImGui::Columns(1);
				ImGui::Separator();
				
			}
			);
			DrawComponent<LightComponent>("Light", entity,
				[&](auto& component)
				{
					static const char* lighttype[] = { "Directional","Point","Spot" };
					static int selectedItem = 0;
					if (ImGui::Combo("LightType", &selectedItem, lighttype, IM_ARRAYSIZE(lighttype)))
					{
						LightComponent::LightType type = LightComponent::LightType(selectedItem);
						if (type != component.type)
						{
							component.type = type;
							component.Validate();
						}
							
					}
					ImGui::DragFloat3("Albedo", glm::value_ptr(component.diffuse));
					ImGui::DragFloat3("Ambient", glm::value_ptr(component.ambient));
					ImGui::DragFloat3("Specular", glm::value_ptr(component.specular));
					static bool shadowmap = false;
					ImGui::Checkbox("See ShadowMap", &shadowmap);
					if (shadowmap)
					{
						if (component.type == LightComponent::LightType::Point)
						{
							uv1 = ImVec2(0.0f, 1.0f);
							uv2 = ImVec2(1.0f, 0.0f);
						}
						ImGui::Image(component.debugMap->GetTexture(), ImVec2(256, 256), uv1, uv2);
						if (component.type == LightComponent::LightType::Point)
						{
							static int selectIndex = 0;
							static const char* index[] = { "X","-X","Y","-Y","Z","-Z" };
							if (ImGui::Combo("TextureIndex", &selectIndex, index, IM_ARRAYSIZE(index)))
							{
								component.textureindex = selectIndex;
							}
						}
					}

					DrawVec3Control("LightDirection", component.lightDirection);
					ImGui::DragFloat("Constant", &component.constant, 0.1f);
					ImGui::DragFloat("Linear", &component.linear, 0.1f);
					ImGui::DragFloat("Quadratic", &component.quadratic, 0.1f);
					ImGui::DragFloat("CutOff", &component.cutoff, 0.1f);
					ImGui::DragFloat("OuterCutOff", &component.outerCutOff, 0.1f);
					ImGui::DragFloat("Radius", &component.radius, 0.1f);
					
				}
			);
	}
	void OpenModel()
	{
		std::optional<std::string> filepath = FileDialogs::OpenFile("Model File \0*.FBX;*.DAE;*.OBJ;\0");
		if (filepath)
		{
			ModelLibrary::LoadModel(*filepath);
		}
	}
	void OpenTexture(Ref<Texture>& texture)
	{
		Sampler_Desc desc;
		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;
		std::optional<std::string> filepath = FileDialogs::OpenFile("Image File \0*.PNG;*.JPEG;*.JPG;\0");
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