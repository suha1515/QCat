#include "Face.h"
#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Noemral;
		glm::vec2 TexCoord;
	};
	Face::Face(const glm::vec3& position, const Ref<Shader>& shader,uint32_t size)
		:translation(position), rotation(glm::vec3(0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f))
		, material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f)
	{
		Ref<Texture2D> diffuseTexture = Texture2D::Create("Asset/textures/floor.png", true);
		material.SetTexture(diffuseTexture, Material::MaterialType::Diffuse);

		Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
		byte r = 125;
		byte g = 125;
		byte b = 125;
		byte a = 125;

		//unsigned int whiteTextureData = 0xffffffff;
		unsigned int whiteTextureData = r << 24 | g << 16 | b << 8 | a;

		whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
		material.SetTexture(whiteTexture, Material::MaterialType::Specular);


		// VertexArray
		m_VertexArray = VertexArray::Create();

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(24 * sizeof(Vertex));

		Vertex vertex[4];
		vertex[0] = { {-0.5f*size,0.0f,-0.5f*size},{0.0f,1.0f,0.0f},{0.0f,0.0f} };
		vertex[1] = { { 0.5f*size,0.0f,-0.5f*size},{0.0f,1.0f,0.0f},{1.0f,0.0f} };
		vertex[2] = { { 0.5f*size,0.0f, 0.5f*size},{0.0f,1.0f,0.0f},{1.0f,1.0f} };
		vertex[3] = { {-0.5f*size,0.0f, 0.5f*size},{0.0f,1.0f,0.0f},{0.0f,1.0f} };

		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[6] = {
			0,1,2, 2,3,0
		};

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));

		// layout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			}, shader));


		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->UnBind();
	}

	void Face::SetScale(const glm::vec3& scale)
	{
		this->scale = scale;
	}

	void Face::SetRotation(const glm::vec3& rotation)
	{
		this->rotation = rotation;
	}

	void Face::Draw(const Ref<Shader>& shader)
	{
		shader->Bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		shader->SetMat4("u_Transform", transform);
		shader->SetMat4("u_invTransform", glm::inverse(transform));

		// material
		shader->SetFloat("material.shininess", material.shininess);

		material.Bind();

		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}

	void Face::ImguiRender(const char* name)
	{
		ImGui::Begin(name);

		ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);

		ImGui::Text("Material");

		ImGui::ColorEdit3("specular", glm::value_ptr(material.specular));
		ImGui::DragFloat("shininess", &material.shininess, 0.03f, 0.1f);


		ImGui::End();
	}

}
