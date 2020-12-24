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
		Ref<DX11Shader> shader = std::dynamic_pointer_cast<QCat::DX11Shader>(s_data->FlatColorShader);

		squareVB->SetLayout(QCat::BufferLayout::Create(
			{ { QCat::ShaderDataType::Float3, "Position" } },
			shader->GetVerexData().data(),
			shader->GetVerexData().size()
		));

		QCat::Ref<QCat::VertexConstantBuffer> m_viewProjection = std::make_shared<QCat::VertexConstantBuffer>(*pGfx,0u,sizeof(glm::mat4));
		QCat::Ref<QCat::VertexConstantBuffer> m_transform = std::make_shared<QCat::VertexConstantBuffer>(*pGfx,1u, sizeof(glm::mat4));
		QCat::Ref<QCat::PixelConstantBuffer> m_color = std::make_shared<QCat::PixelConstantBuffer>(*pGfx,0u,sizeof(glm::vec4));;

		shader->AddVertexConstantBuffer("u_ViewProjection", m_viewProjection);
		shader->AddVertexConstantBuffer("u_Transform", m_transform);
		shader->AddPixelConstantBuffer("u_Color", m_color);
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
		Ref<DX11Shader> shader = std::dynamic_pointer_cast<QCat::DX11Shader>(s_data->FlatColorShader);
		shader->UpdateVertexConstantBuffer("u_ViewProjection", &camera.GetViewProjectionMatrix());
		const glm::mat4 transform = glm::mat4(1.0f);
		shader->UpdateVertexConstantBuffer("u_Transform", &transform);
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#elif defined(QCAT_OPENGL)
		std::dynamic_pointer_cast<OpenGLShader>(s_data->FlatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_data->FlatColorShader)->UploadUniformMat4("u_ViewProjection",camera.GetViewProjectionMatrix());
		std::dynamic_pointer_cast<OpenGLShader>(s_data->FlatColorShader)->UploadUniformMat4("u_Transform", glm::mat4(1.0f));
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
#if defined(QCAT_DX11)
		Ref<DX11Shader> shader = std::dynamic_pointer_cast<QCat::DX11Shader>(s_data->FlatColorShader);
		shader->UpdatePixelConstantBuffer("u_Color", &color);
		shader->Bind();
#elif defined(QCAT_OPENGL)
		std::dynamic_pointer_cast<OpenGLShader>(s_data->FlatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_data->FlatColorShader)->UploadUniformFloat4("u_Color", color);
#endif
		s_data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->QuadVertexArray);
	}
}


