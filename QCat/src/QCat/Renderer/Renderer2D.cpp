#include "qcpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include "API/DirectX11/QGfxDeviceDX11.h"

namespace QCat
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
		// TODO: color, texid, etc..
	};

	struct Renderer2DData
	{
		static const unsigned int MaxQuads = 20000;
		static const unsigned int MaxVertices = MaxQuads * 4;
		static const unsigned int MaxIndices = MaxQuads * 6;
		static const unsigned int MaxTextureSlot = 32; // TODO : textureSlot depends on which device use

		Ref<VertexArray>  QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> whiteTexture;

		unsigned int QuadIndexCount=0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr  = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlot> TextureSlots;
		unsigned int TextureSlotIndex = 1;

		glm::vec4 QuadVertexPosition[4];

		Renderer2D::Statistics Stats;
	};
	static Renderer2DData s_data;

	void Renderer2D::Init()
	{
		QCAT_PROFILE_FUNCTION();
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

		int32_t samplers[s_data.MaxTextureSlot];
		for (uint32_t i = 0; i < s_data.MaxTextureSlot; ++i)
			samplers[i] = i;

#if defined(QCAT_DX11)
		s_data.TextureShader = Shader::Create("Texture", "Asset/shaders/hlsl/Square_VS.hlsl", "Asset/shaders/hlsl/Square_PS.hlsl");
#elif defined(QCAT_OPENGL)
		s_data.TextureShader = Shader::Create("Asset/shaders/glsl/Texture.glsl");
		s_data.TextureShader->Bind();
		s_data.TextureShader->SetIntArray("u_Textures", samplers, s_data.MaxTextureSlot);
#endif
		s_data.QuadVertexBuffer->SetLayout(BufferLayout::Create({
			{ ShaderDataType::Float3, "a_Position"},
			{ ShaderDataType::Float4, "a_Color"},
			{ ShaderDataType::Float2, "a_Texcoord" },
			{ ShaderDataType::Float,  "a_TexIndex"},
			{ ShaderDataType::Float,  "a_TilingFactor"} },
			s_data.TextureShader
		));
		s_data.QuadVertexArray->AddVertexBuffer(s_data.QuadVertexBuffer);

		s_data.QuadVertexBufferBase = new QuadVertex[s_data.MaxIndices];

		s_data.TextureSlots[0] = s_data.whiteTexture;

		s_data.QuadVertexPosition[0] = { -0.5f,-0.5f,0.0f,1.0f };
		s_data.QuadVertexPosition[1] = {  0.5f,-0.5f,0.0f,1.0f };
		s_data.QuadVertexPosition[2] = {  0.5f, 0.5f,0.0f,1.0f };
		s_data.QuadVertexPosition[3] = { -0.5f, 0.5f,0.0f,1.0f };

	}
	void Renderer2D::Shutdown()
	{
		QCAT_PROFILE_FUNCTION();

		delete[] s_data.QuadVertexBufferBase;
	}
	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);
		s_data.TextureShader->Bind();
		s_data.TextureShader->SetMat4("u_ViewProjection", viewProj);
		StartBatch();
	}
	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetViewProjection();

		s_data.TextureShader->Bind();
		s_data.TextureShader->SetMat4("u_ViewProjection", viewProj);
		StartBatch();
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{	
		QCAT_PROFILE_FUNCTION();
		s_data.TextureShader->Bind();
		s_data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartBatch();
	}
	void Renderer2D::EndScene()
	{
		QCAT_PROFILE_FUNCTION();

		Flush();
	}
	void Renderer2D::Flush()
	{
		if (s_data.QuadIndexCount == 0)
			return;
		uint32_t dataSize = (uint32_t)((uint8_t*)s_data.QuadVertexBufferPtr - (uint8_t*)s_data.QuadVertexBufferBase);
		s_data.QuadVertexBuffer->SetData(s_data.QuadVertexBufferBase, dataSize);
		//Bind Texture
		for (uint32_t i = 0; i < s_data.TextureSlotIndex; ++i)
			s_data.TextureSlots[i]->Bind(i);
		RenderCommand::DrawIndexed(s_data.QuadVertexArray,s_data.QuadIndexCount);

		s_data.Stats.DrawCalls++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x,position.y,0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		QCAT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; //White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_data.QuadVertexBufferPtr->Position = transform * s_data.QuadVertexPosition[i];
			s_data.QuadVertexBufferPtr->Color = color;
			s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_data.QuadVertexBufferPtr++;
		}

		s_data.QuadIndexCount += 6;
		s_data.QuadVertexArray->Bind();

		s_data.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,  float tilingFactor, const glm::vec4& tintColor )
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,  float tilingFactor, const glm::vec4& tintColor)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, texture, tilingFactor,tintColor);

	}
	void Renderer2D::DrawQuad(const glm::mat4& transform,  const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		QCAT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.f;
		for (uint32_t i = 1; i < s_data.TextureSlotIndex; ++i)
		{
			if (*s_data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_data.TextureSlotIndex >= Renderer2DData::MaxTextureSlot)
				NextBatch();
			textureIndex = (float)s_data.TextureSlotIndex;
			s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
			s_data.TextureSlotIndex++;
		}
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_data.QuadVertexBufferPtr->Position = transform * s_data.QuadVertexPosition[i];
			s_data.QuadVertexBufferPtr->Color = tintColor;
			s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_data.QuadVertexBufferPtr++;
		}

		s_data.QuadIndexCount += 6;
		s_data.QuadVertexArray->Bind();

		s_data.Stats.QuadCount++;
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x,position.y,0.0f }, size, rotation,texture,tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		QCAT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f,0.0f,1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x,size.y,1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_data.Stats;
	}
	void Renderer2D::ResetStats()
	{
		memset(&s_data.Stats, 0, sizeof(Statistics));
	}
	void Renderer2D::StartBatch()
	{
		s_data.QuadIndexCount = 0;
		s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

		s_data.TextureSlotIndex = 1;
	}
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}


