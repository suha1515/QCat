#include "Sandbox2D.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"


#include <chrono>
#include <QCat/InputDevice/Mouse/Mouse.h>



namespace QCat
{

	Sandbox2D::Sandbox2D()
		:Layer("Sandbox2D")
	{
	}

	void Sandbox2D::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			geometryShaderTest = Shader::Create("Asset/shaders/glsl/GeometryShader/TestGeometry.glsl");
		}
		else
		{

		}
	
		vao = VertexArray::Create();

		float points[] = {
			-0.5f,  0.5f,1.0f,0.0f,0.0f, // top-left
			 0.5f,  0.5f,0.0f,1.0f,0.0f, // top-right
			 0.5f, -0.5f,0.0f,0.0f,1.0f, // bottom-right
			-0.5f, -0.5f,1.0f,1.0f,0.0f  // bottom-left
		};

		Ref<VertexBuffer> buffer = VertexBuffer::Create(points, sizeof(points));

		unsigned int index[] =
		{
			0,1,2,3
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(index, 4);

		buffer->SetLayout(BufferLayout::Create(
			{
				{ShaderDataType::Float2,"a_Position"},
				{ShaderDataType::Float3,"a_Color"}
			}, geometryShaderTest
		));

		vao->AddVertexBuffer(buffer);
		vao->SetIndexBuffer(indexBuffer);
		//
		RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1.0f });
	}

	void Sandbox2D::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void Sandbox2D::OnUpdate(QCat::Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		RenderCommand::Clear();

		geometryShaderTest->Bind();
		vao->Bind();
		RenderCommand::DrawIndexed(vao, vao->GetIndexBuffer()->GetCount(),RenderAPI::DrawMode::POINT);
		geometryShaderTest->UnBind();

	}

	void Sandbox2D::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");
	
		ImGui::End();

	
	}

	void Sandbox2D::OnEvent(QCat::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));


	}

	bool Sandbox2D::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();
		
		return false;
	}

}
