#include "CubeMap.h"

namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
	};
	CubeMap::CubeMap(const std::vector<std::string> imgPathes, const Ref<Shader> shader)
	{
		m_cubeTexture = TextureCube::Create(imgPathes);
		// VertexArray
		m_VertexArray = VertexArray::Create();

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(36 * sizeof(Vertex));
		Vertex vertex[36];
		// front
		vertex[0] = { {-1.0f,-1.0f,-1.0f} };
		vertex[1] = { { 1.0f, 1.0f,-1.0f} };
		vertex[2] = { { 1.0f,-1.0f,-1.0f} };

		vertex[3] = { {-1.0f,-1.0f,-1.0f} };
		vertex[4] = { {-1.0f, 1.0f,-1.0f} };
		vertex[5] = { { 1.0f, 1.0f,-1.0f} };

		// left							
		vertex[6] = { { 1.0f,-1.0f,-1.0f} };
		vertex[7] = { { 1.0f, 1.0f, 1.0f} };
		vertex[8] = { { 1.0f,-1.0f, 1.0f} };

		vertex[9] = { { 1.0f,-1.0f,-1.0f} };
		vertex[10] = { { 1.0f, 1.0f,-1.0f} };
		vertex[11] = { { 1.0f, 1.0f, 1.0f} };

		// backward						 
		vertex[12] = { { 1.0f,-1.0f, 1.0f} };
		vertex[13] = { {-1.0f, 1.0f, 1.0f} };
		vertex[14] = { {-1.0f,-1.0f, 1.0f} };

		vertex[15] = { { 1.0f, -1.0f,1.0f} };
		vertex[16] = { { 1.0f, 1.0f, 1.0f} };
		vertex[17] = { {-1.0f, 1.0f, 1.0f} };

		// right						 
		vertex[18] = { {-1.0f,-1.0f, 1.0f} };
		vertex[19] = { {-1.0f, 1.0f,-1.0f} };
		vertex[20] = { {-1.0f,-1.0f,-1.0f} };

		vertex[21] = { {-1.0f,-1.0f, 1.0f} };
		vertex[22] = { {-1.0f, 1.0f, 1.0f} };
		vertex[23] = { {-1.0f, 1.0f,-1.0f} };

		//top							  
		vertex[24] = { {-1.0f,1.0f, -1.0f} };
		vertex[25] = { { 1.0f,1.0f,  1.0f} };
		vertex[26] = { { 1.0f,1.0f, -1.0f} };

		vertex[27] = { {-1.0f,1.0f, -1.0f} };
		vertex[28] = { {-1.0f,1.0f,  1.0f} };
		vertex[29] = { { 1.0f,1.0f,  1.0f} };

		// bottom						
		vertex[30] = { { 1.0f, -1.0f,-1.0} };
		vertex[31] = { {-1.0f, -1.0f, 1.0} };
		vertex[32] = { {-1.0f, -1.0f,-1.0} };

		vertex[33] = { {  1.0f,-1.0f,-1.0} };
		vertex[34] = { {  1.0f,-1.0f, 1.0} };
		vertex[35] = { { -1.0f,-1.0f, 1.0} };

		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[36] = {
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
			,24,25,26,27,28,29,30,31,32,33,34,35
		};


		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));

		// layout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"}
			}, shader));


		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->UnBind();
	}
	void CubeMap::Draw(const Ref<Shader>& shader)
	{
		m_VertexArray->Bind();
		m_cubeTexture->Bind();
		shader->UpdateBuffer();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
}

