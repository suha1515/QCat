
#include <QCat.h>

// ---- Entry Point----//
#include "QCat/Core/EntryPoint.h"
// --------------------//
#include <Imgui/imgui.h>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Blender.h"

#include "QCat/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>

#include "../Header/Sandbox2D.h"
#include "../Header/ModelTestScene.h"
#include "../Header/TestScene.h"
#include "../Header/PbrTest.h"
#include "../Header/RenderGraphTest.h"
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
		ImGui::ShowDemoWindow();

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
		:Application("QCat App",QCat::RenderAPI::API::DirectX11)
	{
		//PushLayer(new ExamLayer());
		PushLayer(new QCat::ModelTestScene());
		//PushLayer(new QCat::Sandbox2D());
		//PushLayer(new QCat::TestScene());
		//PushLayer(new QCat::PbrTest());
		//PushLayer(new QCat::RenderGraphTest());
	}
	~Sandbox()
	{
	}

};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}