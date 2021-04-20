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

	Sphere::Sphere(const glm::vec3& position,float radius, int sectorCount, int stackCount)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f))
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
		// generate CW index list of sphere triangles
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
		for (int i = 0; i < indices.size(); i += 3)
		{
			// position
			glm::vec3 pos1 = vertices[indices[i]].Position;
			glm::vec3 pos2 = vertices[indices[i+1]].Position;
			glm::vec3 pos3 = vertices[indices[i+2]].Position;
			// texture
			glm::vec2 uv1 = vertices[indices[i]].TexCoord;
			glm::vec2 uv2 = vertices[indices[i+1]].TexCoord;
			glm::vec2 uv3 = vertices[indices[i+2]].TexCoord;

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

			vertices[indices[i]].Tangent = tangent1;
			vertices[indices[i]].BiTangent = bitangent1;

			vertices[indices[i+1]].Tangent = tangent1;
			vertices[indices[i+1]].BiTangent = bitangent1;

			vertices[indices[i+2]].Tangent = tangent1;
			vertices[indices[i+2]].BiTangent = bitangent1;
		}
		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		vertexBuffer->SetData(vertices.data(), vertices.size() *sizeof(Vertex));

		// IndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());


		// Shader for layout signature
		Ref<Shader> vertexShader;
		if(RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		   vertexShader = ShaderLibrary::Load("Blinn-Phong","Asset/shaders/hlsl/Blinn-Phong.hlsl");

		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"  },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float3, "a_Tangent"},
			  { ShaderDataType::Float3, "a_BiTangent"}
			}, vertexShader
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

	const glm::mat4& Sphere::GetTransform()
	{
		// TODO: 여기에 return 문을 삽입합니다.
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		return transform;
	}

	void Sphere::Draw()
	{
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Sphere::ImguiRender(const char* name)
	{
	
	}


}
