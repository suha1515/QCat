#include "Mesh.h"

namespace QCat
{
	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Material material, const Ref<Shader>& shader)
	{
		this->material = material;

		Initialize(shader, vertices,indices);
	}

	void Mesh::Draw(const Ref<Shader>& shader,const glm::mat4& transform)
	{
		// material
		shader->SetFloat("material.shininess", material.shininess, ShaderType::PS);

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
			  {ShaderDataType::Float3, "a_BiTangent"}
			}, shader
		));

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		

		//unbind
		m_VertexArray->UnBind();
	}
}

