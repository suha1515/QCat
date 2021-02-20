#include "Sphere.h"
#include <Imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "../Light/Light.h"
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

	Sphere::Sphere(const glm::vec3& position, const Ref<Shader>& shader, float radius, int sectorCount, int stackCount)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f)),
		material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f)
	{
		//material.SetTexture("Asset/textures/container2.png", Material::MaterialType::Diffuse);
		//material.SetTexture("Asset/textures/container2_specular.png", Material::MaterialType::Specular);
		//material.SetTexture("Asset/textures/blackpng.png", Material::MaterialType::Emission);

		//shader->Bind();
		//shader->SetInt("material.diffuse", 0);
		//shader->SetInt("material.specular", 1);
		// VertexArray
		m_VertexArray = VertexArray::Create();

		std::vector<Vertex> vertices;
		float x, y, z, xy;
		float nx, ny, nz, lengthInv = 1.0f / radius;
		float s, t;

		constexpr float PI = glm::pi<float>();
		float sectorStep = 2 * PI / sectorCount;	//Sector means longitude(가로)
		float stackStep = PI / stackCount;			// stack means latitude (세로)
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;	// starting from pi/2 ~ -pi/2 90~-90 (180)
			xy = radius * cosf(stackAngle);			// r * cos(u)
			y = radius * sinf(stackAngle);			// r * sin(u);

			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;

				// vertex Position (x,y,z)
				x = xy * cosf(sectorAngle);
				z = xy * sinf(sectorAngle);

				// normal Position (nx,ny,nz)

				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;

				// vertex Texcoord (s,t)
				s = (float)j / sectorCount;
				t = (float)i / stackCount;

				vertices.emplace_back(Vertex{ {x,y,z},{nx,ny,nz },{s,t}});
			}
		}
		// generate CCW index list of sphere triangles
		// k1--k1+1
		// |  / |
		// | /  |
		// k2--k2+1

		std::vector<uint32_t> indices;
		std::vector<uint32_t> lineIndices;

		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);	//beginning of first stack
			k2 = k1 + sectorCount + 1; // beginning of nex stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2triagle per sector excluding first and last stacks
				// k1>= k2 >= k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k1+1);
					indices.push_back(k2);
				}
				// k1+1 >= k2 >= k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(k1 +1);
					indices.push_back(k2+1);
					indices.push_back(k2);
				}
				// store indices for lines
				// vertical lines for all stacks, k1>=k2
				lineIndices.push_back(k1);
				lineIndices.push_back(k2);
				if (i != 0)	// horizontal lines except 1st stack , k1 => k+1
				{
					lineIndices.push_back(k1);
					lineIndices.push_back(k1+1);
				}
			}
		}
		for (int i = 0; i < indices.size(); i += 6)
		{
			// position
			glm::vec3 pos1 = vertices[indices[i]].Position;
			glm::vec3 pos2 = vertices[indices[i+1]].Position;
			glm::vec3 pos3 = vertices[indices[i+2]].Position;
			glm::vec3 pos4 = vertices[indices[i+5]].Position;
			// texture
			glm::vec2 uv1 = vertices[indices[i]].TexCoord;
			glm::vec2 uv2 = vertices[indices[i+1]].TexCoord;
			glm::vec2 uv3 = vertices[indices[i+2]].TexCoord;
			glm::vec2 uv4 = vertices[indices[i+5]].TexCoord;

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

			vertices[indices[i]].Tangent = tangent1;
			vertices[indices[i]].BiTangent = bitangent1;

			vertices[indices[i+1]].Tangent = tangent1;
			vertices[indices[i+1]].BiTangent = bitangent1;

			vertices[indices[i+2]].Tangent = tangent1;
			vertices[indices[i+2]].BiTangent = bitangent1;

			vertices[indices[i+3]].Tangent = tangent2;
			vertices[indices[i+3]].BiTangent = bitangent2;

			vertices[indices[i+4]].Tangent = tangent2;
			vertices[indices[i+4]].BiTangent = bitangent2;

			vertices[indices[i+5]].Tangent = tangent2;
			vertices[indices[i+5]].BiTangent = bitangent2;
		}
		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		vertexBuffer->SetData(vertices.data(), vertices.size() *sizeof(Vertex));

		// IndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent"},
			  { ShaderDataType::Float3, "a_BiTangent"}
			}, shader
		));

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->UnBind();
	}

	void Sphere::SetScale(const glm::vec3& scale)
	{
		this->scale = scale;
	}

	void Sphere::SetRotation(const glm::vec3& rotation)
	{
		this->rotation = rotation;
	}

	void Sphere::Draw(const Ref<Shader>& shader)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
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
	void Sphere::ImguiRender(const char* name)
	{
		ImGui::Begin(name);

		ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);

		ImGui::Text("Material");

		ImGui::ColorEdit3("ambient", glm::value_ptr(material.ambient));
		ImGui::ColorEdit3("diffuse", glm::value_ptr(material.diffuse));
		ImGui::ColorEdit3("specular", glm::value_ptr(material.specular));
		ImGui::DragFloat("shininess", &material.shininess, 0.03f, 0.1f);


		ImGui::End();
	}


}
