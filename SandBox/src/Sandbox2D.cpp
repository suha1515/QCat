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
	{
		QCAT_PROFILE_SCOPE("Renderer Prep");
		QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		QCat::RenderCommand::Clear();
	}
	
	{
		QCAT_PROFILE_SCOPE("Renderer Draw");
		QCat::Renderer2D::BeginScene(m_CameraController.GetCamera());
#if defined(QCAT_DX11)
		QCat::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.999f }, { 10.0f, 10.0f }, m_Texture,10.0f);
		QCat::Renderer2D::DrawRotatedQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, glm::radians(-45.0f), { 0.8f,0.2f,0.3f,1.0f });
		QCat::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.3f,0.8f,1.0f });
#elif defined(QCAT_OPENGL)
		QCat::Renderer2D::DrawQuad({ 0.0f, 0.0f,-0.1f }, { 10.0f, 10.0f }, m_Texture,10.0f);
		QCat::Renderer2D::DrawRotatedQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, glm::radians(-45.0f), { 0.8f,0.2f,0.3f,1.0f });
		QCat::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.3f,0.8f,1.0f });
#endif
		QCat::Renderer2D::EndScene();
	}

}

void Sandbox2D::OnImGuiRender()
{
	QCAT_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(QCat::Event& e)
{
	m_CameraController.OnEvent(e);
}
