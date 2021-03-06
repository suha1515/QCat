
#include <QCat.h>

// ---- Entry Point----//
#include "QCat/Core/EntryPoint.h"
// --------------------//
#include <Imgui/imgui.h>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Blender.h"

#include "QCat/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"
#include "ModelTestScene.h"
class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example")
	{

	}
	void OnUpdate(QCat::Timestep step) override
	{
		////Update
		//m_CameraController.OnUpdate(step);

		////Render
		//QCat::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1 });
		//QCat::RenderCommand::Clear();

		//QCat::Renderer::BeginScene(m_CameraController.GetCamera());

		//QCat::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override
	{
		//ImGui::Begin("Settings");
		//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		//ImGui::End();
	}
	void OnEvent(QCat::Event& e) override
	{
		//m_CameraController.OnEvent(e);
	}

private:

};

class Sandbox : public QCat::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExamLayer());
		PushLayer(new QCat::ModelTestScene());
	}
	~Sandbox()
	{
	}

};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}