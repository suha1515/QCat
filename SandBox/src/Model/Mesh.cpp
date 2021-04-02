#include "Mesh.h"

namespace QCat
{
	Mesh::Mesh(const glm::mat4& transform,std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const Ref<Shader>& shader)
	{
		this->transform = transform;
		Initialize(shader, vertices,indices);
	}

	void Mesh::Draw(const Ref<Shader>& shader,const glm::mat4& transform)
	{
		glm::mat4 finalTransform = transform * this->transform;
		shader->SetMat4("u_Transform", finalTransform, ShaderType::VS);
		shader->SetMat4("u_invTransform", glm::inverse(finalTransform), ShaderType::VS);

		m_VertexArray->Bind();
		shader->UpdateBuffer();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Mesh::Initialize(const Ref<Shader>& shader, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
	{
		// VertexArray Object Creation
		m_VertexArray = VertexArray::Create();
		// make vertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));

		vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent" },
			  {ShaderDataType::Float3,  "a_BiTangent"}
			}, shader
		));

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		

		//unbind
		m_VertexArray->UnBind();
	}
}

