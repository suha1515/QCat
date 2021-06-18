#include "Cube.h"
#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace QCat
{
	static float a = 1.0f;
	static float b = 1.0f;
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Noemral;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
		glm::vec3 BiTangent;
	};
	Cube::Cube(const glm::vec3& position)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f))
	{

		// VertexArray
		m_VertexArray = VertexArray::Create("Cube");

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(36 * sizeof(Vertex));
		Vertex vertex[36];
		// backward
		float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? 1.0f : 0.0f;
		 bias = 0.0f;
		vertex[0] = { { -1.0f, -1.0f, -1.0f},{0.0f,0.0f,-1.0f},{0.0f,1.0 - bias} };
		vertex[1] = { {  1.0f,  1.0f, -1.0f},{0.0f,0.0f,-1.0f},{1.0f,0.0 + bias} };
		vertex[2] = { {  1.0f, -1.0f, -1.0f},{0.0f,0.0f,-1.0f},{1.0f - bias,1.0} };

		vertex[3] = { { 1.0f,  1.0f, -1.0f},{0.0f,0.0f,-1.0f},{0.0f,1.0 - bias} };
		vertex[4] = { {-1.0f, -1.0f, -1.0f},{0.0f,0.0f,-1.0f},{0.0f,0.0f + bias} };
		vertex[5] = { {-1.0f,  1.0f, -1.0f},{0.0f,0.0f,-1.0f},{1.0f,0.0f + bias}};

		// left
		vertex[6]  ={ { -1.0f,  1.0f,  1.0f},{1.0f,0.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[7] = { { -1.0f,  1.0f, -1.0f},{1.0f,0.0f,0.0f},{1.0f,0.0 + bias} };
		vertex[8] = { { -1.0f, -1.0f, -1.0f},{1.0f,0.0f,0.0f},{1.0f-bias,1.0}};

		vertex[9]  = { { -1.0f, -1.0f, -1.0f},{1.0f,0.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[10] = { { -1.0f, -1.0f,  1.0f},{1.0f,0.0f,0.0f},{0.0f,0.0f + bias} };
		vertex[11] = { { -1.0f,  1.0f,  1.0f},{1.0f,0.0f,0.0f},{1.0f,0.0f+bias}};
		
		// forawrd
		vertex[12] = { { -1.0f, -1.0f,  1.0f},{0.0f,0.0f,1.0f},{0.0f,1.0 - bias} };
		vertex[13] = { {  1.0f, -1.0f,  1.0f},{0.0f,0.0f,1.0f},{1.0f,0.0 + bias} };
		vertex[14] = { {  1.0f,  1.0f,  1.0f},{0.0f,0.0f,1.0f},{1.0f - bias,1.0} };

		vertex[15] = { { 1.0f,  1.0f,  1.0f},{0.0f,0.0f,1.0f},{0.0f,1.0 - bias} };
		vertex[16] = { {-1.0f,  1.0f,  1.0f},{0.0f,0.0f,1.0f},{0.0f,0.0f + bias} };
		vertex[17] = { {-1.0f, -1.0f,  1.0f},{0.0f,0.0f,1.0f},{1.0f,0.0f + bias} };
		
		// right
		vertex[18] = { {1.0f,  1.0f,  1.0f},{-1.0f,0.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[19] = { {1.0f, -1.0f, -1.0f},{-1.0f,0.0f,0.0f},{1.0f,0.0 + bias} };
		vertex[20] = { {1.0f,  1.0f, -1.0f},{-1.0f,0.0f,0.0f},{1.0f - bias,1.0} };

		vertex[21] = { {1.0f, -1.0f, -1.0f},{-1.0f,0.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[22] = { {1.0f,  1.0f,  1.0f},{-1.0f,0.0f,0.0f},{0.0f,0.0f + bias} };
		vertex[23] = { {1.0f, -1.0f,  1.0f},{-1.0f,0.0f,0.0f},{1.0f,0.0f + bias} };
		
		//top
		vertex[24] = { {-1.0f,  1.0f, -1.0f},{0.0f,1.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[25] = { { 1.0f,  1.0f , 1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0 + bias} };
		vertex[26] = { { 1.0f,  1.0f, -1.0f},{0.0f,1.0f,0.0f},{1.0f - bias,1.0} };

		vertex[27] = { { 1.0f,  1.0f,  1.0f},{0.0f,1.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[28] = { {-1.0f,  1.0f, -1.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f + bias} };
		vertex[29] = { {-1.0f,  1.0f,  1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f + bias} };

		// bottom
		vertex[30] = { {-1.0f, -1.0f, -1.0f},{0.0f,-1.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[31] = { { 1.0f, -1.0f, -1.0f},{0.0f,-1.0f,0.0f},{1.0f,0.0 + bias} };
		vertex[32] = { { 1.0f, -1.0f,  1.0f},{0.0f,-1.0f,0.0f},{1.0f - bias,1.0} };

		vertex[33] = { { 1.0f, -1.0f,  1.0f},{0.0f,-1.0f,0.0f},{0.0f,1.0 - bias} };
		vertex[34] = { {-1.0f, -1.0f,  1.0f},{0.0f,-1.0f,0.0f},{0.0f,0.0f + bias} };
		vertex[35] = { {-1.0f, -1.0f, -1.0f},{0.0f,-1.0f,0.0f},{1.0f,0.0f + bias} };
		
		for (int i = 0; i < 36; i += 3)
		{
			// position
			glm::vec3 pos1 = vertex[i].Position;
			glm::vec3 pos2 = vertex[i + 1].Position;
			glm::vec3 pos3 = vertex[i + 2].Position;
			// texture
			glm::vec2 uv1 = vertex[i].TexCoord;
			glm::vec2 uv2 = vertex[i + 1].TexCoord;
			glm::vec2 uv3 = vertex[i + 2].TexCoord;

			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			glm::vec3 tangent1, bitangent1;
			glm::vec3 tangent2, bitangent2;
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			//triangle 1
			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


			vertex[i].Tangent = tangent1;
			vertex[i].BiTangent = bitangent1;

			vertex[i + 1].Tangent = tangent1;
			vertex[i + 1].BiTangent = bitangent1;

			vertex[i + 2].Tangent = tangent1;
			vertex[i + 2].BiTangent = bitangent1;
		}

		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[36] = {
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
			,24,25,26,27,28,29,30,31,32,33,34,35
		};

		
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));

		Ref<Shader> vertexShader;
		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			vertexShader = ShaderLibrary::Load("Blinn-Phong", "Asset/shaders/hlsl/Blinn-Phong.hlsl");
		// layout
		vertexBuffer->SetLayout(BufferLayout::Create(
				{ { ShaderDataType::Float3, "a_Position"},
				  { ShaderDataType::Float3, "a_Normal"},
				  { ShaderDataType::Float2, "a_TexCoord"},
				  { ShaderDataType::Float3, "a_Tangent"},
				  { ShaderDataType::Float3, "a_BiTangent"},
				}, vertexShader));
		

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->UnBind();

		MeshLibrary::Set(m_VertexArray->GetMeshName(), m_VertexArray);
	}
	void Cube::SetScale(const glm::vec3& scale)
	{
		this->scale = scale;
	}
	void Cube::SetRotation(const glm::vec3& rotation)
	{
		this->rotation = rotation;
	}
	void Cube::SetTranslation(const glm::vec3& translation)
	{
		this->translation = translation;
	}
	const glm::mat4 Cube::GetTransform()
	{
		return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
	}
	void Cube::Draw()
	{
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Cube::ImguiRender(const char* name)
	{
		
	}
}