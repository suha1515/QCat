
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
class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example"), m_CameraController(1280.0f/720.0f,true)
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
	QCat::ShaderLibrary m_ShaderLibrary;
	QCat::Ref<QCat::Shader> m_Shader;
	QCat::Ref<QCat::VertexArray> m_VertexArray;

	QCat::Ref<QCat::Shader> m_FlatColorShader;
	QCat::Ref<QCat::VertexArray> m_SquareVA;

	QCat::Ref<QCat::Texture2D> m_Texture,m_Texture2;

	//DX11
	QCat::Ref<QCat::VertexBuffer> m_VertexBuffer;
	QCat::Ref<QCat::IndexBuffer> m_IndexBuffer;

	QCat::Ref<QCat::VertexBuffer> m_SquareBuffer;
	QCat::Ref<QCat::IndexBuffer> m_SquareIndex;

	QCat::Ref<QCat::DX11Blender> m_BlenderState;

	//dx11 buffer
	QCat::Ref<QCat::VertexConstantBuffer> m_constantBuffer;
	QCat::Ref<QCat::VertexConstantBuffer> m_transform;
	QCat::Ref<QCat::PixelConstantBuffer> m_color;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	//camera
	QCat::OrthographicCameraController m_CameraController;

};

class Sandbox : public QCat::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExamLayer());
		PushLayer(new QCat::Sandbox2D());
	}
	~Sandbox()
	{
	}
};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}