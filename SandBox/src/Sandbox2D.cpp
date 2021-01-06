#include "Sandbox2D.h"

#include "imgui.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	QCAT_PROFILE_FUNCTION();

	m_Texture = QCat::Texture2D::Create("Asset/textures/Checkerboard.png");
	

#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
}

void Sandbox2D::OnDetach()
{
	QCAT_PROFILE_FUNCTION();

}
void Sandbox2D::OnUpdate(QCat::Timestep ts)
{
	QCAT_PROFILE_FUNCTION();
	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	// Reset stats here
	QCat::Renderer2D::ResetStats();
	{
		QCAT_PROFILE_SCOPE("Renderer Prep");
		QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		QCat::RenderCommand::Clear();
	}
	{
		QCAT_PROFILE_SCOPE("Renderer Draw");
		static float rotation = 0.0f;
		rotation += ts * 50.0f;
//#if defined(QCAT_DX11)
//#elif defined(QCAT_OPENGL)
//#endif
		QCat::Renderer2D::BeginScene(m_CameraController.GetCamera());
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
				glm::vec4 color = { (x + 5.0f) / 10.0f,0.4f,(y + 5.0f) / 10.0f,0.7f};
				QCat::Renderer2D::DrawQuad({ x,y }, { 0.45f,0.45f }, color);
			}
		}
		QCat::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	QCAT_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = QCat::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(QCat::Event& e)
{
	m_CameraController.OnEvent(e);
}
