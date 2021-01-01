#include "qcpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "RenderCommand.h"

namespace QCat
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		// TODO: color, texid, etc..
	};

	struct Renderer2DData
	{
		const unsigned int MaxQuads = 10000;
		const unsigned int MaxVertices = MaxQuads * 4;
		const unsigned int MaxIndices = MaxQuads * 6;

		Ref<VertexArray>  QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;

		unsigned int QuadIndexCount=0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr  = nullptr;
	};
	static Renderer2DData s_data;

	void Renderer2D::Init()
	{
		QCAT_PROFILE_FUNCTION();
		int bias = 0;
#if defined(QCAT_OPENGL)
		//bias = -1;
#elif defined(QCAT_DX11)
		//vertices
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f,0.0f, 1.0f + bias,
			 0.5f, -0.5f, 0.0f,1.0f, 1.0f + bias,
			 0.5f,  0.5f, 0.0f,1.0f, 0.0f - bias,
			-0.5f,  0.5f, 0.0f,0.0f, 0.0f - bias
		};
#endif
		s_data.QuadVertexArray = VertexArray::Create();

		const void* extraData = nullptr;

		//vertexBuffer
		s_data.QuadVertexBuffer = VertexBuffer::Create(s_data.MaxVertices * sizeof(QuadVertex));
		
		unsigned int* quadIndices = new unsigned int[s_data.MaxIndices];

		unsigned int offset = 0;
		for (unsigned int i = 0; i < s_data.MaxIndices; i+=6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		//indexBuffer
		Ref<IndexBuffer> quadIB;
		quadIB = IndexBuffer::Create(quadIndices,s_data.MaxIndices);
		s_data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[]quadIndices;
		
		s_data.whiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xffffffff;
		s_data.whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));

#if defined(QCAT_DX11)
		s_data.TextureShader = Shader::Create("Texture", "..\\bin\\Debug-windows-\\SandBox\\Square_VS.cso", "..\\bin\\Debug-windows-\\SandBox\\Square_PS.cso");
#elif defined(QCAT_OPENGL)
		s_data.TextureShader = Shader::Create("Asset/shaders/glsl/Texture.glsl");
		s_data.TextureShader->Bind();
		s_data.TextureShader->SetInt("u_Texture", 0);
#endif
		s_data.QuadVertexBuffer->SetLayout(BufferLayout::Create({
			{ ShaderDataType::Float3, "a_Position"},
			{ ShaderDataType::Float4, "a_Color"},
			{ ShaderDataType::Float2, "a_Texcoord" } },
			s_data.TextureShader
		));
		s_data.QuadVertexArray->AddVertexBuffer(s_data.QuadVertexBuffer);

		s_data.QuadVertexBufferBase = new QuadVertex[s_data.MaxIndices];
	}
	void Renderer2D::Shutdown()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{	
		QCAT_PROFILE_FUNCTION();

		s_data.TextureShader->Bind();
#if defined(QCAT_DX11)
		s_data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#elif defined(QCAT_OPENGL)
		s_data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
#endif
		s_data.QuadIndexCount = 0;
		s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;
	}
	void Renderer2D::EndScene()
	{
		QCAT_PROFILE_FUNCTION();

		//upload data to gpu
		unsigned int datasize = (unsigned char*)s_data.QuadVertexBufferPtr - (unsigned char*)s_data.QuadVertexBufferBase;
		s_data.QuadVertexBuffer->SetData(s_data.QuadVertexBufferBase, datasize);

		Flush();
	}
	void Renderer2D::Flush()
	{
		RenderCommand::DrawIndexed(s_data.QuadVertexArray,s_data.QuadIndexCount);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		QCAT_PROFILE_FUNCTION();

		s_data.QuadVertexBufferPtr->Position = position;
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = {0.0f,0.0f};
		s_data.QuadVertexBufferPtr++;

		s_data.QuadVertexBufferPtr->Position = { position.x + size.x,position.y , 0.0f };
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = { 1.0f,0.0f };
		s_data.QuadVertexBufferPtr++;

		s_data.QuadVertexBufferPtr->Position = { position.x + size.x,position.y + size.y, 0.0f };
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = { 1.0f,1.0f };
		s_data.QuadVertexBufferPtr++;

		s_data.QuadVertexBufferPtr->Position = { position.x ,position.y + size.y, 0.0f };
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexCoord = { 0.0f,1.0f };
		s_data.QuadVertexBufferPtr++;

		s_data.QuadIndexCount += 6;
		//glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
		//	* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
#if defined(QCAT_DX11)
		//s_data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		//s_data.whiteTexture->Bind();
#elif defined(QCAT_OPENGL)
		//s_data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		//s_data.TextureShader->SetMat4("u_Transform", transform);
		//s_data.whiteTexture->Bind();
#endif
		s_data.QuadVertexArray->Bind();
		//RenderCommand::DrawIndexed(s_data.QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,  float tilingFactor, const glm::vec4& tintColor )
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,  float tilingFactor, const glm::vec4& tintColor)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data.TextureShader->SetFloat4("u_Color", tintColor);
		s_data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		s_data.TextureShader->SetMat4("u_Transform", transform);
		texture->Bind();

		s_data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data.QuadVertexArray);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f),rotation,{0.0f,0.0f,1.0f})
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });
#if defined(QCAT_DX11)
		s_data.TextureShader->SetFloat4("u_Color", color);
		s_data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_data.TextureShader->SetMat4("u_Transform", transform);
		s_data.whiteTexture->Bind();
#elif defined(QCAT_OPENGL)
		s_data.TextureShader->SetFloat4("u_Color", color);
		s_data.TextureShader->SetFloat("u_TilingFactor", 1.0f);
		s_data.TextureShader->SetMat4("u_Transform", transform);
		s_data.whiteTexture->Bind();
#endif
		s_data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data.QuadVertexArray);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation,texture,tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data.TextureShader->SetFloat4("u_Color", tintColor /*{ 0.2f,0.3f,0.8f,0.5f }*/);
		s_data.TextureShader->SetFloat("u_TilingFactor", tilingFactor);
		s_data.TextureShader->SetMat4("u_Transform", transform);
		texture->Bind();

		s_data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_data.QuadVertexArray);
	}
}


