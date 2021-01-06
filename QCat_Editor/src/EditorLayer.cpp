#include "EditorLayer.h"

#include "imgui.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>

namespace QCat
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_Texture = QCat::Texture2D::Create("Asset/textures/Checkerboard.png");

		QCat::FrameBufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = QCat::FrameBuffer::Create(fbSpec);
#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
	}

	void EditorLayer::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void EditorLayer::OnUpdate(QCat::Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		// Update
		m_CameraController.OnUpdate(ts);
		// Render
		// Reset stats here
		QCat::Renderer2D::ResetStats();
		{
			QCAT_PROFILE_SCOPE("Renderer Prep");

			m_Framebuffer->Bind();
			QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			QCat::RenderCommand::Clear();
		}
		{
			QCAT_PROFILE_SCOPE("Renderer Draw");
			QCat::Renderer2D::BeginScene(m_CameraController.GetCamera());
			static float rotation = 0.0f;
			rotation += ts * 50.0f;
#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
			QCat::Renderer2D::DrawRotatedQuad({ 1.0f,0.0f }, { 0.8f,0.8f }, -45.0f, { 0.8f,0.2f,0.3f,1.0f });
			QCat::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
			QCat::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.3f,0.8f,1.0f });
			QCat::Renderer2D::DrawQuad({ 0.0f,0.0f,0.1f }, { 20.0f, 20.0f }, m_Texture, 10.0f);
			QCat::Renderer2D::DrawRotatedQuad({ -2.0f,0.0f,0.0f }, { 0.8f,0.8f }, rotation, m_Texture, 20.0f);
			QCat::Renderer2D::EndScene();

			QCat::Renderer2D::BeginScene(m_CameraController.GetCamera());
			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f,0.4f,(y + 5.0f) / 10.0f,0.7f };
					QCat::Renderer2D::DrawQuad({ x,y }, { 0.45f,0.45f }, color);

				}
			}
			QCat::Renderer2D::EndScene();
			//if (QCat::Input::IsKeyPressed(QCAT_KEY_SPACE))
			//{
			//	m_Framebuffer->SaveColorBuffer();
			//}
			m_Framebuffer->UnBind();
			QCat::RenderCommand::SetDefaultFrameBuffer();
		}

	}

	void EditorLayer::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Exit")) QCat::Application::GetInstance().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = QCat::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		ImGui::Image(m_Framebuffer->GetColorAttachmentRendererID(), ImVec2(1280.f, 720.f), ImVec2{ 0,1 }, ImVec2{ 1,0 });
		//ImGui::Image(m_Texture->GetTexture(), ImVec2(256.f, 256.f));

		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::OnEvent(QCat::Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}
