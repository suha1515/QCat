#include "EditorLayer.h"
#include <Imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QCat/Scene/SceneSerializer.h>
#include <QCat/Renderer/RenderAPI.h>
#include <QCat/Uitiliy/PlatformUtils.h>
#include <QCat/Math/Math.h>

#include "Model/ModelLoader.h"

namespace QCat
{
	extern const std::filesystem::path g_AssetPath;

	void SetMaterial(Entity& entity, Material& mat,Scene* activeScene)
	{
		if (entity.HasComponent<MaterialComponent>())
		{
			entity.GetComponent<MaterialComponent>().material = mat;
		}
		auto childid = entity.GetComponent<RelationShipComponent>().firstChild;
		auto childentity = activeScene->GetEntityById(childid);
		while (childentity != Entity::emptyEntity)
		{
			SetMaterial(childentity, mat,activeScene);
			childentity = activeScene->GetEntityById(childentity.GetComponent<RelationShipComponent>().nextSibling);
		}
	}
	void UpdateTransform(Entity& entity, const glm::mat4 parentMatrix, Scene* activeScene)
	{
		TransformComponent& transcomp = entity.GetComponent<TransformComponent>();
		transcomp.parentMatrix = parentMatrix;
		const glm::mat4 localMatrix = transcomp.GetTransform();
		auto childid = entity.GetComponent<RelationShipComponent>().firstChild;
		auto childentity = activeScene->GetEntityById(childid);

		while (childentity != Entity::emptyEntity)
		{
			UpdateTransform(childentity, localMatrix,activeScene);
			childentity = activeScene->GetEntityById(childentity.GetComponent<RelationShipComponent>().nextSibling);
		}
	}
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
		
	}

	void EditorLayer::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();
		Cube::Initialize();
		Sphere::Initialize();

		Sampler_Desc desc;

		Texture_Desc texDesc;
		texDesc.Width = 1280;
		texDesc.Height = 720;
		texDesc.MipLevels = 1;
		texDesc.SampleCount = 1;

		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;

		NewScene();

		m_EditorCamera = EditorCamera(60.f, 1.778f, 0.01f,100.0f);

		m_HoveredEntity = Entity();
		hdrImage = TextureLibrary::Load("Asset/textures/HdrImage/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", desc);

		viewMatrix = CreateRef<glm::mat4>(glm::mat4(1.0f));
		projectionMatrix = CreateRef<glm::mat4>(glm::mat4(1.0f));
		camData = CreateRef<glm::vec4>(1.0f);

		EditorPBRRenderGraph.SetHdrImg(hdrImage);
		EditorPBRRenderGraph.SetView(viewMatrix);
		EditorPBRRenderGraph.SetProj(projectionMatrix);
		EditorPBRRenderGraph.SetCameraProperties(camData);
		EditorPBRRenderGraph.Initialize();

		Sampler_Desc imgSamp;
		imgSamp.addressU = WrapingMode::REPEAT;
		imgSamp.addressV = WrapingMode::REPEAT;
		imgSamp.MIN = Filtering::LINEAR;
		imgSamp.MAG = Filtering::LINEAR;
		imgSamp.MIP = Filtering::NONE;
	}

	void EditorLayer::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void EditorLayer::OnUpdate(Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		
		// Resize
		if (m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(EditorPBRRenderGraph.width != m_ViewPortSize.x || EditorPBRRenderGraph.height != m_ViewPortSize.y))
		{
			//m_FrameBufferEx->GetTexture("ColorBuffer1")->SetSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			//m_FrameBufferEx->GetTexture("DepthBuffer")->SetSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
				//Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			EditorPBRRenderGraph.SetSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_CameraController.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
			m_EditorCamera.SetViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
			m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			camData->x = m_EditorCamera.GetFov();
			camData->y = m_EditorCamera.GetAspectRatio();
			camData->z = m_EditorCamera.GetNear();
			
		}

		// Update
		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(ts);	
			m_EditorCamera.OnUpdate(ts);
		}

		*viewMatrix = m_EditorCamera.GetViewMatrix();
		*projectionMatrix = m_EditorCamera.GetProjection();

		EditorPBRRenderGraph.Execute(m_ActiveScene);

		EditorPBRRenderGraph.SetDebugMode(m_ProjectpropertyPanel.m_IsDebugCascade);
		EditorPBRRenderGraph.SetCascadeSplits(m_ProjectpropertyPanel.m_CascadeSplitsLamda);
		EditorPBRRenderGraph.SetSoftShadow(m_ProjectpropertyPanel.m_SoftShadow);
		camData->w = m_ProjectpropertyPanel.m_ShadowMaxDistance;
		// Update Scene
		//m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera); 
		m_ActiveScene->OnUpdateRuntime(ts);

		
	
	}

	void EditorLayer::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control
				
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save as...", "Crtl+Shift+S"))
					SaveSceneAs();


				if (ImGui::MenuItem("Exit")) Application::GetInstance().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		m_ContentBrowserPanel.OnImGuiRender();
		m_SceneHierarchyPanel.OnImguiRender();
		m_ProjectpropertyPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("ViewPort");
		auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportoffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportoffset.x,viewportMinRegion.y + viewportoffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportoffset.x,viewportMaxRegion.y + viewportoffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::GetInstance().GetImguiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelsize = ImGui::GetContentRegionAvail();
		m_ViewPortSize = { viewportPanelsize.x,viewportPanelsize.y };

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			ImGui::Image(EditorPBRRenderGraph.GetColorBuffer()->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			ImGui::Image(EditorPBRRenderGraph.GetColorBuffer()->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BRWOSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath)/ path);
			}
		
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x
				, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Camera
			// Runtime Camera for entity
			/*auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const glm::mat4& cameraProjection = camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			*/

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Sanapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;
			// snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue,snapValue,snapValue };

			// use all data
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),nullptr,nullptr,snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				glm::mat4 localTransform = glm::inverse(tc.parentMatrix) * transform ;
				Math::DecomposeTransform(localTransform,translation, rotation, scale);

				glm::vec3 deltaRotation = rotation -  tc.Rotation;
				//tc.SetTranslation(translation);
				//tc.SetRotation(tc.Rotation + deltaRotation);
				//tc.SetScale(scale);
				if (tc.Translation != translation)
				{
					tc.Translation = translation;
					tc.changed = true;
				}
				if (tc.Rotation != tc.Rotation + deltaRotation)
				{
					tc.Rotation += deltaRotation;
					tc.changed = true;
				}
				if (tc.Scale != scale)
				{
					tc.Scale = scale;
					tc.changed = true;
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();

	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		/*if (e.GetEventType() == EventType::KeyPressed)
		{
			QCAT_CORE_TRACE("key : {0}", e);
		}*/
	}
	void EditorLayer::CameraUpdate(QCat::Timestep ts)
	{
		cameraSpeed = 2.5f * ts;
		auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;
		// camera roration by mouse
		static bool fpsmode = false;
		if (fpsmode)
		{
			while (const auto delta = Input::GetDeltaData())
			{
				float dx = (float)delta->x;
				float dy = (float)delta->y;


				pitch -= dy * 0.04f;
				glm::vec3 front;

				{
					front = { 0.0f,0.0f,1.0f };
					yaw -= dx * 0.04f;
				}

				//QCAT_INFO("dx : {0} , dy{1}", dx, dy);

				if (pitch > 89.0f)
					pitch = 89.f;
				if (pitch < -89.0f)
					pitch = -89.f;


				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);
			}
			// camera Move
			if (Input::IsKeyPressed(Key::W))
			{
				tc += cameraSpeed * cameraFront;
			}
			if (Input::IsKeyPressed(Key::S))
			{
				tc -= cameraSpeed * cameraFront;
			}
			if (Input::IsKeyPressed(Key::A))
			{
				tc -= cameraSpeed * cameraRight;
			}
			if (Input::IsKeyPressed(Key::D))
			{
				tc += cameraSpeed * cameraRight;
			}
			if (Input::IsKeyPressed(Key::Escape))
				fpsmode = false;
		}
		else
		{
			if (Input::IsKeyPressed(Key::Escape))
				fpsmode = true;
		}

		cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront));
		cameraUp = glm::cross(cameraFront, cameraRight);
		*viewMatrix = glm::lookAt(tc, tc + cameraFront, cameraUp);
	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortscuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewScene();
			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();
			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();
			break;
		}
		
		// Gizmo
		case Key::Q:
			if(!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		case Key::W:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft || e.GetMouseButton() == Mouse::ButtonRight)
		{
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;
			glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

			// opengl texture origin is bottom left so we have to flip y coordinate
			if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
				my = viewportSize.y - my;

			int mouseX = (int)mx;
			int mouseY = (int)my;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				int idvalue=-1;
				std::dynamic_pointer_cast<Texture2D>(EditorPBRRenderGraph.GetIDBuffer())->ReadData(0, mouseX, mouseY, 1, 1, TextureFormat::RED32_INTEGER, TextureDataType::INT, 4, &idvalue);
				m_HoveredEntity = idvalue == -1 ? Entity() : m_ActiveScene->GetEntityById(idvalue);
			}

			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				if (m_HoveredEntity != Entity::emptyEntity)
				{
					std::string entityname = m_HoveredEntity.GetComponent<TagComponent>().Tag;
					QCAT_WARN("Select Entity Name : {0}", entityname);
					m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity, false);

				}
			}	
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		TextureLibrary::Reset();
		MeshLibrary::Reset();
		Cube::Initialize();
		Sphere::Initialize();
		Sampler_Desc desc;
		TextureLibrary::Load("CheckerBoard", "Asset/textures/Checkerboard.png", desc);
		
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ContentBrowserPanel.ReloadImage();
	}

	void EditorLayer::OpenScene()
	{
		std::optional<std::string> filepath = FileDialogs::OpenFile("QCat Scene (*.QScene)\0*.QScene\0");
		if (filepath)
		{
			OpenScene(*filepath);
		}
		
	}
	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		std::string filepath = path.string();
		auto begin = filepath.find_last_of('.');
		std::string extension = filepath.substr(begin + 1, filepath.length());
		if (extension != "QScene")
		{
			QCAT_CORE_ERROR("Wrong Extension for LoadScene! Wrong Extension : {0}", extension);
		}
		else
		{
			NewScene();

			SceneSerializer serializer(m_ActiveScene);
			serializer.DeSerialize(path.string());
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::optional<std::string> filepath = FileDialogs::SaveFile("QCat Scene (*.QScene)\0*.QScene\0");
		if (filepath)
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(*filepath);
		}
	}
}
