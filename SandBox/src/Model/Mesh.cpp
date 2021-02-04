#include "Mesh.h"

namespace QCat
{
	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, const Ref<Shader>& shader)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->m_Textures = textures;

		Initialize(shader);
	}

	void Mesh::Draw(const Ref<Shader>& shader)
	{
		shader->Bind();
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (uint32_t i = 0; i < m_Textures.size(); ++i)
		{
			std::string number;
			std::string name = m_Textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			shader->SetInt("material." + name + number, i);
			m_Textures[i].texture->Bind(i);
		}

		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Mesh::Initialize(const Ref<Shader>& shader)
	{
		// VertexArray Object Creation
		m_VertexArray = VertexArray::Create();
		// make vertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));

		vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

		// makeIndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"}
			}, shader
		));

		//unbind
		m_VertexArray->UnBind();
	}
}

