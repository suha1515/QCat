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

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f));
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		Sampler_Desc desc;
		m_Texture = Texture2D::Create("Asset/textures/Checkerboard.png",desc);

		AttachmentSpecification fbSpecEx= { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer1"},
										    {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RED32_INTEGER,"IndexBuffer"},
										    {FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer"} };
		fbSpecEx.Width = 0;
		fbSpecEx.Height = 0;
		m_FrameBufferEx = FrameBufferEx::Create(fbSpecEx);

		Texture_Desc texDesc;
		texDesc.Width = 1280;
		texDesc.Height = 720;
		texDesc.MipLevels = 1;
		texDesc.SampleCount = 1;

		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;

		m_FrameBufferEx->InitializeTexture("ColorBuffer1", texDesc, desc);
		m_FrameBufferEx->InitializeTexture("IndexBuffer", texDesc, desc);
		m_FrameBufferEx->InitializeTexture("DepthBuffer", texDesc, desc);

		m_ActiveScene = CreateRef<Scene>();
		
		m_EditorCamera = EditorCamera(60.f, 1.778f, 0.1f, 1000.0f);
		//m_EditorCamera.SetPosition({ 0.0f,0.0f,-2.0f });
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_HoveredEntity = Entity();
		hdrImage = TextureLibrary::Load("Asset/textures/HdrImage/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", desc,1, 1);

		viewMatrix = CreateRef<glm::mat4>(glm::mat4(1.0f));
		projectionMatrix = CreateRef<glm::mat4>(glm::mat4(1.0f));

		EditorPBRRenderGraph.SetHdrImg(hdrImage);
		EditorPBRRenderGraph.SetView(viewMatrix);
		EditorPBRRenderGraph.SetProj(projectionMatrix);
		EditorPBRRenderGraph.Initialize();

		m_Camera = m_ActiveScene->CreateEntity("Camera");

		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 0.0f,0.0f,-2.0f };

		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(1, 0.01f, 100.0f);

		Entity light1 = m_ActiveScene->CreateEntity("PointLight");

		light1.GetComponent<TransformComponent>().Translation = glm::vec3(10.0f, 10.0f, -20.0f);
		auto& comp = light1.AddComponent<LightComponent>();
		comp.diffuse = { 300.0f,300.0f,300.0f };

		Sampler_Desc imgSamp;
		imgSamp.addressU = WrapingMode::REPEAT;
		imgSamp.addressV = WrapingMode::REPEAT;
		imgSamp.MIN = Filtering::LINEAR;
		imgSamp.MAG = Filtering::LINEAR;
		imgSamp.MIP = Filtering::NONE;
		/*Material goldenBall;
		goldenBall.SetTexture("Asset/textures/PBR/gold/albedo.png", imgSamp, Material::TextureType::Diffuse);
		goldenBall.SetTexture("Asset/textures/PBR/gold/normal.png", imgSamp, Material::TextureType::Normal);
		goldenBall.SetTexture("Asset/textures/PBR/gold/metallic.png", imgSamp, Material::TextureType::Metallic);
		goldenBall.SetTexture("Asset/textures/PBR/gold/roughness.png", imgSamp, Material::TextureType::Roughness);
		goldenBall.SetTexture("Asset/textures/PBR/gold/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		Entity ball = m_ActiveScene->CreateEntity("GoldenBall");
		ball.GetComponent<TransformComponent>().Translation = { -1.0f,0.0f,0.0f };
		ball.AddComponent<MeshComponent>("Cube");
		ball.AddComponent<MaterialComponent>(goldenBall);*/

		model = ModelLoader::LoadModel("Asset/model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX", m_ActiveScene);

		Material gunMat;
		imgSamp.MIP = Filtering::NONE;
		gunMat.SetTexture("Asset/model/Cerberus_by_Andrew_Maximov/textures/Cerberus_A.tga", imgSamp, Material::TextureType::Diffuse);
		gunMat.SetTexture("Asset/model/Cerberus_by_Andrew_Maximov/textures/Cerberus_M.tga", imgSamp, Material::TextureType::Metallic);
		gunMat.SetTexture("Asset/model/Cerberus_by_Andrew_Maximov/textures/Cerberus_N.tga", imgSamp, Material::TextureType::Normal);
		gunMat.SetTexture("Asset/model/Cerberus_by_Andrew_Maximov/textures/Cerberus_R.tga", imgSamp, Material::TextureType::Roughness);

		SetMaterial(model, gunMat,m_ActiveScene.get());
		model.GetComponent<TransformComponent>().Scale = { 0.01f,0.01f,0.01f };
		model.GetComponent<TransformComponent>().Rotation = { 3.2f,1.6f,0.0f };
		model.GetComponent<TransformComponent>().Translation = { 0.0,-0.4f,-1.0f };
		UpdateTransform(model, glm::mat4(1.0f), m_ActiveScene.get());
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
		}

		// Update
		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(ts);	
		}

		m_EditorCamera.OnUpdate(ts);
		*viewMatrix = m_EditorCamera.GetViewMatrix();
		*projectionMatrix = m_EditorCamera.GetProjection();
		//UpdateTransform(model, glm::mat4(1.0f));
		// Render
		// Reset stats here
		//m_Framebuffer->Bind();
		//m_Framebuffer->Clear();
		
		/*m_FrameBufferEx->Bind();
		m_FrameBufferEx->AttachTexture("ColorBuffer1", AttachmentType::Color_0, TextureType::Texture2D,0);
		m_FrameBufferEx->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		m_FrameBufferEx->Clear();*/
		EditorPBRRenderGraph.Execute(m_ActiveScene);
		// clear out entity ID attacment to -1
		//int value = -1;
		//m_Framebuffer->ClearAttachment(1, &value);

		// Update Scene
		//m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera); 
		m_ActiveScene->OnUpdateRuntime(ts);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		// opengl texture origin is bottom left so we have to flip y coordinate
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		/*if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData;
			pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}*/
		
		
		//m_Framebuffer->UnBind();
		//m_FrameBufferEx->UnBind();
		//RenderCommand::SetDefaultFrameBuffer();
	
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

		m_SceneHierarchyPanel.OnImguiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("ViewPort");
		//auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar

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
	/*	if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			ImGui::Image(m_FrameBufferEx->GetTexture("ColorBuffer1")->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			ImGui::Image(m_FrameBufferEx->GetTexture("ColorBuffer1")->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y));
		}*/
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			ImGui::Image(EditorPBRRenderGraph.GetColorBuffer()->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			ImGui::Image(EditorPBRRenderGraph.GetColorBuffer()->GetTexture(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y));
		}

		/*auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x,minBound.y };
		m_ViewportBounds[1] = { maxBound.x,maxBound.y };*/


		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			/*float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth,windowHeight);*/

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
			glm::mat4 delttransform;

			// Sanapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;
			// snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue,snapValue,snapValue };

			// use all data
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),glm::value_ptr(delttransform),nullptr,snap ? snapValues : nullptr);

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
			if(m_ViewportHovered&& !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::optional<std::string> filepath = FileDialogs::OpenFile("QCat Scene (*.QScene)\0*.QScene\0");
		if (filepath)
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.DeSerialize(*filepath);
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
