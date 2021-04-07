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
		glm::vec3 Tangent;
		glm::vec3 BiTangent;
	};
	Face::Face(const glm::vec3& position, const Ref<Shader>& shader, Material material,uint32_t size)
		:translation(position), rotation(glm::vec3(0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f))
		, material(material)
	{
		// VertexArray
		m_VertexArray = VertexArray::Create();

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(24 * sizeof(Vertex));

		Vertex vertex[6];
		vertex[0] = { {-0.5f*size,-0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,1.0f} };
		vertex[1] = { {-0.5f*size, 0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,0.0f} };
		vertex[2] = { { 0.5f*size, 0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,0.0f} };
							
		vertex[3] = { {-0.5f*size,-0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,1.0f} };
		vertex[4] = { { 0.5f*size, 0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,0.0f} };
		vertex[5] = { { 0.5f*size,-0.5f*size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,1.0f} };

	/*	vertex[0] = { {-0.5f * size,-0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,0.0f} };
		vertex[1] = { {-0.5f * size, 0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,1.0f} };
		vertex[2] = { { 0.5f * size, 0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,1.0f} };

		vertex[3] = { {-0.5f * size,-0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{0.0f,0.0f} };
		vertex[4] = { { 0.5f * size, 0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,1.0f} };
		vertex[5] = { { 0.5f * size,-0.5f * size,0.0f,},{0.0f,0.0f,-1.0f},{1.0f,0.0f} };*/

		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		// position
		glm::vec3 pos1 = vertex[0].Position;
		glm::vec3 pos2 = vertex[1].Position;
		glm::vec3 pos3 = vertex[2].Position;
		glm::vec3 pos4 = vertex[5].Position;
		// texture
		glm::vec2 uv1 = vertex[0].TexCoord;
		glm::vec2 uv2 = vertex[1].TexCoord;
		glm::vec2 uv3 = vertex[2].TexCoord;
		glm::vec2 uv4 = vertex[5].TexCoord;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		vertex[0].Tangent = tangent1;
		vertex[0].BiTangent = bitangent1;

		vertex[1].Tangent = tangent1;
		vertex[1].BiTangent = bitangent1;

		vertex[2].Tangent = tangent1;
		vertex[2].BiTangent = bitangent1;

		vertex[3].Tangent = tangent2;
		vertex[3].BiTangent = bitangent2;

		vertex[4].Tangent = tangent2;
		vertex[4].BiTangent = bitangent2;

		vertex[5].Tangent = tangent2;
		vertex[5].BiTangent = bitangent2;

		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[6] = {
			0,1,2,3,4,5
		};

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));

		// layout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal" },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent"},
			  { ShaderDataType::Float3, "a_BiTangent"},
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

	void Face::SetTranslation(const glm::vec3& translation)
	{
		this->translation = translation;
	}

	void Face::Draw(const Ref<Shader>& shader)
	{
		
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		shader->SetMat4("u_Transform", transform, ShaderType::VS);
		shader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);

		// material
		shader->SetFloat("material.shininess", material.shininess, ShaderType::PS);

		if (material.IsThereTexture(Material::TextureType::Normal))
		{
			shader->SetBool("material.normalMap", true, ShaderType::PS);
		}
		else
		{
			shader->SetBool("material.normalMap", false, ShaderType::PS);
		}

		m_VertexArray->Bind();
		shader->UpdateBuffer();
		RenderCommand::DrawIndexed(m_VertexArray);
	}

	void Face::MaterialBind()
	{
		material.Bind();
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
