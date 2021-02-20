#include "Cube.h"
#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "../Light/Light.h"
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
	Cube::Cube(const glm::vec3& position,const Ref<Shader>& shader)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f))
		,material(glm::vec3(1.0f,0.5f,0.31f),glm::vec3(1.0f,0.5f,0.31f),glm::vec3(0.5f,0.5f,0.5f),32.0f)
	{

		/*Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
		unsigned int whiteTextureData = 0xffffffff;
		whiteTexture->SetData(&whiteTextureData, sizeof(unsigned int));
		material.SetTexture(whiteTexture, Material::MaterialType::Specular);*/


		// VertexArray
		m_VertexArray = VertexArray::Create();

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(36 * sizeof(Vertex));
		Vertex vertex[36];
		// front
		vertex[0] = { {-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{0.0f,1.0f} };
		vertex[1] = { { 0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f} };
		vertex[2] = { { 0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,1.0f} };

		vertex[3] = { {-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{0.0f,1.0f} };
		vertex[4] = { {-0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{0.0f,0.0f} };
		vertex[5] = { { 0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f} };

		// left
		vertex[6] = { { 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f} };
		vertex[7] = { { 0.5f, 0.5f, 0.5f},{1.0f,0.0f,0.0f},{1.0f,0.0f} };
		vertex[8] = { { 0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f} };

		vertex[9] = { { 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f} };
		vertex[10] = { { 0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,0.0f} };
		vertex[11] = { { 0.5f, 0.5f, 0.5f},{1.0f,0.0f,0.0f},{1.0f,0.0f} };
		
		// backward
		vertex[12] = { { 0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0f,1.0f} };
		vertex[13] = { {-0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{1.0f,0.0f} };
		vertex[14] = { {-0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f} };

		vertex[15] = { { 0.5f, -0.5f,0.5f},{0.0f,0.0f,1.0f},{0.0f,1.0f} };
		vertex[16] = { { 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0f,0.0f} };
		vertex[17] = { {-0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{1.0f,0.0f} };
		
		// right
		vertex[18] = { {-0.5f,-0.5f, 0.5f},{-1.0f,0.0f,0.0f},{0.0f,1.0f} };
		vertex[19] = { {-0.5f, 0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,0.0f} };
		vertex[20] = { {-0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f} };

		vertex[21] = { {-0.5f,-0.5f, 0.5f},{-1.0f,0.0f,0.0f},{0.0f,1.0f} };
		vertex[22] = { {-0.5f, 0.5f, 0.5f},{-1.0f,0.0f,0.0f},{0.0f,0.0f} };
		vertex[23] = { {-0.5f, 0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,0.0f} };
		
		//top
		vertex[24] = { {-0.5f,0.5f, -0.5f},{0.0f,1.0f,0.0f},{0.0f,1.0f} };
		vertex[25] = { { 0.5f,0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,0.0f} };
		vertex[26] = { { 0.5f,0.5f, -0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f} };

		vertex[27] = { {-0.5f,0.5f, -0.5f},{0.0f,1.0f,0.0f},{0.0f,1.0f} };
		vertex[28] = { {-0.5f,0.5f,  0.5f},{0.0f,1.0f,0.0f},{0.0f,0.0f} };
		vertex[29] = { { 0.5f,0.5f,  0.5f},{0.0f,1.0f,0.0f},{1.0f,0.0f} };

		// bottom
		vertex[30] = { { 0.5f, -0.5f,-0.5f},{0.0f,-1.0f,0.0f},{0.0f,1.0f} };
		vertex[31] = { {-0.5f, -0.5f, 0.5f},{0.0f,-1.0f,0.0f},{1.0f,0.0f} };
		vertex[32] = { {-0.5f, -0.5f,-0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f} };

		vertex[33] = { {  0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{0.0f,1.0f} };
		vertex[34] = { {  0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{0.0f,0.0f} };
		vertex[35] = { { -0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{1.0f,0.0f} };
		
		for (int i = 0; i < 36; i += 6)
		{
			// position
			glm::vec3 pos1 = vertex[i].Position;
			glm::vec3 pos2 = vertex[i + 1].Position;
			glm::vec3 pos3 = vertex[i + 2].Position;
			glm::vec3 pos4 = vertex[i + 4].Position;
			// texture
			glm::vec2 uv1 = vertex[i].TexCoord;
			glm::vec2 uv2 = vertex[i + 1].TexCoord;
			glm::vec2 uv3 = vertex[i + 2].TexCoord;
			glm::vec2 uv4 = vertex[i + 4].TexCoord;

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

			//triangle 2
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

			vertex[i].Tangent = tangent1;
			vertex[i].BiTangent = bitangent1;

			vertex[i + 1].Tangent = tangent1;
			vertex[i + 1].BiTangent = bitangent1;

			vertex[i + 2].Tangent = tangent1;
			vertex[i + 2].BiTangent = bitangent1;

			vertex[i + 3].Tangent = tangent2;
			vertex[i + 3].BiTangent = bitangent2;

			vertex[i + 4].Tangent = tangent2;
			vertex[i + 4].BiTangent = bitangent2;

			vertex[i + 5].Tangent = tangent2;
			vertex[i + 5].BiTangent = bitangent2;
		}

		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[36] = {
			0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
			,24,25,26,27,28,29,30,31,32,33,34,35
		};

		
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));

		// layout
		vertexBuffer->SetLayout(BufferLayout::Create(
				{ { ShaderDataType::Float3, "a_Position"},
				  { ShaderDataType::Float3, "a_Normal"},
				  { ShaderDataType::Float2, "a_TexCoord"},
				  { ShaderDataType::Float3, "a_Tangent"},
				  { ShaderDataType::Float3, "a_BiTangent"},
				}, shader));
		

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->UnBind();
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
	void Cube::Draw(const Ref<Shader>& shader)
	{
		glm::mat4 transform= glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		shader->SetMat4("u_Transform", transform, ShaderType::VS);
		shader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);

		// material
		shader->SetFloat("material.shininess", material.shininess, ShaderType::PS);


		if (material.IsThereTexture(Material::MaterialType::NormalMap))
		{
			shader->SetBool("material.normalMap", true, ShaderType::PS);
		}
		else
		{
			shader->SetBool("material.normalMap", false, ShaderType::PS);
		}

		material.Bind();
		m_VertexArray->Bind();
		shader->UpdateBuffer();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Cube::ImguiRender(const char* name)
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