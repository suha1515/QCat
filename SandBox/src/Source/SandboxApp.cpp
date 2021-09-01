#include <QCat.h>

// ---- Entry Point----//
#include "QCat/Core/EntryPoint.h"
// --------------------//
#include <Imgui/imgui.h>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Blender.h"

#include "QCat/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>


class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example")
	{

	}
	void OnAttach() override
	{
		QCat::RenderCommand::SetClearColor(glm::vec4(0.3f,0.3f,0.3f,1.0f));
		computeShader = QCat::Shader::Create("Asset/shaders/hlsl/ComputShaderPractice.hlsl");
	}
	void OnUpdate(QCat::Timestep step) override
	{
		QCat::RenderCommand::Clear();

	}
	virtual void OnImGuiRender() override
	{
		//ImGui::ShowDemoWindow();

	}
	void OnEvent(QCat::Event& e) override
	{
		//m_CameraController.OnEvent(e);
	}

private:
	QCat::Ref<QCat::Shader> computeShader;
};

class Sandbox : public QCat::Application
{
public:
	Sandbox()
		:Application("QCat App",QCat::RenderAPI::API::DirectX11)
	{
		PushLayer(new ExamLayer());
	}
	~Sandbox()
	{
	}

};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}