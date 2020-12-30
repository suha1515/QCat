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
		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;
	};
	static Renderer2DStorage* s_data;
	void Renderer2D::Init()
	{
		int bias = 0;
#if defined(QCAT_OPENGL)
		//bias = -1;
#endif
		s_data = new Renderer2DStorage();
		s_data->QuadVertexArray = VertexArray::Create();

		const void* extraData = nullptr;
		//vertices
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f,0.0f, 1.0f+bias,
			 0.5f, -0.5f, 0.0f,1.0f, 1.0f+bias,
			 0.5f,  0.5f, 0.0f,1.0f, 0.0f-bias,
			-0.5f,  0.5f, 0.0f,0.0f, 0.0f-bias
		};
		//vertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//indexBuffer
		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		s_data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_data->whiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xffffffff;
		s_data->whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
#if defined(QCAT_DX11)
		s_data->TextureShader = Shader::Create("Texture", "..\\bin\\Debug-windows-\\SandBox\\Square_VS.cso", "..\\bin\\Debug-windows-\\SandBox\\Square_PS.cso");
#elif defined(QCAT_OPENGL)
		s_data->TextureShader = Shader::Create("Asset/shaders/glsl/Texture.glsl");
		s_data->TextureShader->Bind();
		s_data->TextureShader->SetInt("u_Texture", 0);
#endif
		squareVB->SetLayout(BufferLayout::Create({ 
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_Texture"} },
			s_data->TextureShader
		));
		s_data->QuadVertexArray->AddVertexBuffer(squareVB);
	}
	void Renderer2D::Shutdown()
	{
		delete s_data;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{	
		s_data->TextureShader->Bind();
#if defined(QCAT_DX11)
		s_data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#elif defined(QCAT_OPENGL)
		s_data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
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
		s_data->TextureShader->SetFloat4("u_Color", color);
		s_data->TextureShader->SetMat4("u_Transform", transform);
		s_data->whiteTexture->Bind();
#elif defined(QCAT_OPENGL)
		s_data->TextureShader->SetFloat4("u_Color", color);
		s_data->TextureShader->SetMat4("u_Transform", transform);
		s_data->whiteTexture->Bind();
#endif
		s_data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_data->TextureShader->SetMat4("u_Transform", transform);
		s_data->TextureShader->SetFloat4("u_Color",{ 0.2f,0.3f,0.8f,0.5f });
		texture->Bind();

		s_data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data->QuadVertexArray);
	}
}


