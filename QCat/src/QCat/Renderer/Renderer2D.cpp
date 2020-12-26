#include "qcpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "RenderCommand.h"

namespace QCat
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> FlatColorShader;
	};
	static Renderer2DStorage* s_data;
	void Renderer2D::Init()
	{
		s_data = new Renderer2DStorage();
		s_data->QuadVertexArray = VertexArray::Create();

		//vertices
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		//vertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//indexBuffer
		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		s_data->QuadVertexArray->SetIndexBuffer(squareIB);

#if defined(QCAT_DX11)
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		s_data->FlatColorShader = Shader::Create("Flatcolor", "..\\bin\\Debug-windows-\\SandBox\\flatcolor_VS.cso", "..\\bin\\Debug-windows-\\SandBox\\flatcolor_PS.cso");
		Ref<DXShader> shader = std::dynamic_pointer_cast<QCat::DXShader>(s_data->FlatColorShader);

		squareVB->SetLayout(QCat::BufferLayout::Create(
			{ { QCat::ShaderDataType::Float3, "Position" } },
			shader->GetVerexData().data(),
			shader->GetVerexData().size()
		));

#elif defined(QCAT_OPENGL)
		s_data->FlatColorShader = Shader::Create("Asset/shaders/glsl/FlatColor.glsl");
		squareVB->SetLayout(BufferLayout::Create({
			{ ShaderDataType::Float3, "a_Position" }
			}));	
#endif
		s_data->QuadVertexArray->AddVertexBuffer(squareVB);
	}
	void Renderer2D::Shutdown()
	{
		delete s_data;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{	
#if defined(QCAT_DX11)
		s_data->FlatColorShader->SetMat4u("u_ViewProjection", "viewProj", camera.GetViewProjectionMatrix());
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#elif defined(QCAT_OPENGL)
		s_data->FlatColorShader->Bind();
		s_data->FlatColorShader->SetMat4("u_ViewProjection",camera.GetViewProjectionMatrix());
		
#endif
	}
	void Renderer2D::EndScene()
	{
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
#if defined(QCAT_DX11)
		s_data->FlatColorShader->SetFloat4("u_Color", color);
		s_data->FlatColorShader->SetMat4u("u_Transform", "transform", transform);
		s_data->FlatColorShader->Bind();
#elif defined(QCAT_OPENGL)
		s_data->FlatColorShader->Bind();
		s_data->FlatColorShader->SetFloat4("u_Color", color);
		s_data->FlatColorShader->SetMat4("u_Transform", transform);
#endif
		s_data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->QuadVertexArray);
	}
}


