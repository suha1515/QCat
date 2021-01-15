#include "EditorLayer.h"
#include <Imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QCat/Scene/SceneSerializer.h>

#include <QCat/Renderer/RenderAPI.h>

#include <QCat/Uitiliy/PlatformUtils.h>

namespace QCat
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_Texture = Texture2D::Create("Asset/textures/Checkerboard.png");

		FrameBufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = FrameBuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		
		//auto square = m_ActiveScene->CreateEntity("Green Square");
		//square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,1.0f,0.0f,1.0f });

		//auto Redsquare = m_ActiveScene->CreateEntity("Red Square");
		//Redsquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f,0.0f,0.0f,1.0f });

		//m_SquareEntity = square;

		//m_CameraEntity = m_ActiveScene->CreateEntity("CameraA Entity");
		//m_CameraEntity.AddComponent<CameraComponent>();

		//m_SecondCamera = m_ActiveScene->CreateEntity("CameraB Entity");
		//auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		//cc.Primary = false;

		//class CameraController : public ScriptableEntity
		//{
		//public:
		//	void OnCreate()
		//	{
		//		
		//	}
		//	void OnDestroy()
		//	{

		//	}
		//	void OnUpdate(Timestep ts)
		//	{
		//		auto& translation = GetComponent<TransformComponent>().Translation;
		//		float speed = 5.0f;
		//		if (Input::IsKeyPressed(Key::A))
		//			translation.x -= speed * ts;
		//		if (Input::IsKeyPressed(Key::D))
		//			translation.x += speed * ts;
		//		if (Input::IsKeyPressed(Key::W))
		//			translation.y += speed * ts;
		//		if (Input::IsKeyPressed(Key::S))
		//			translation.y -= speed * ts;
		//	}
		//};

		//m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		//m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
	}

	void EditorLayer::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void EditorLayer::OnUpdate(Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		// Resize
		if (FrameBufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_CameraController.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);

			m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}
		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);
		// Render
		// Reset stats here
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
		// Update Scene
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->UnBind();
		RenderCommand::SetDefaultFrameBuffer();

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

		ImGui::Begin("Renderer2D Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

		ImGui::Begin("ViewPort");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::GetInstance().GetImguiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelsize = ImGui::GetContentRegionAvail();
		m_ViewPortSize = { viewportPanelsize.x,viewportPanelsize.y };
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), ImVec2(m_ViewPortSize.x, m_ViewPortSize.y));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();

	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		/*if (e.GetEventType() == EventType::KeyPressed)
		{
			QCAT_CORE_TRACE("key : {0}", e);
		}*/
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
		std::string filepath = FileDialogs::OpenFile("QCat Scene (*.QScene)\0*.QScene\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportReSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.DeSerialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("QCat Scene (*.QScene)\0*.QScene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
}
