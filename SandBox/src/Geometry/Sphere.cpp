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
	};

	Sphere::Sphere(const glm::vec3& position, const char* texturePath, float radius, int sectorCount, int stackCount)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f)),
		material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f)
	{
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = Shader::Create("Asset/shaders/glsl/FlatShader.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = Shader::Create("SphereShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/Solid_PS.hlsl");
		}
		if (texturePath != "")
			m_Texture = Texture2D::Create(texturePath);
		else
		{
			m_Texture = Texture2D::Create(1, 1);
			unsigned int whiteTextureData = 0xffffffff;
			m_Texture->SetData(&whiteTextureData, sizeof(unsigned int));
		}

		shader->Bind();
		shader->SetInt("u_Texture", 0);

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
					indices.push_back(k2);
					indices.push_back(k1+1);

				}

				// k1+1 >= k2 >= k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(k1 +1);
					indices.push_back(k2);
					indices.push_back(k2+1);
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

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		vertexBuffer->SetData(vertices.data(), vertices.size() *sizeof(Vertex));

		// IndexBuffer
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		// InputLayout
		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"}
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

	void Sphere::Draw(const glm::mat4& cameraTransform,const glm::mat4& proj, LightInfo info)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", proj*glm::inverse(cameraTransform));
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		glm::vec3 viewpos = cameraTransform[3];
		shader->SetMat4("u_Transform", transform);
		shader->SetMat4("u_invTransform", glm::inverse(transform));
		shader->SetFloat3("viewPosition", viewpos);

		// light
		shader->SetFloat3("light.diffuse", info.lightColor);
		shader->SetFloat3("light.ambient", info.lightAmbient);
		shader->SetFloat3("light.position", info.lightPos);
		shader->SetFloat3("light.specular", info.lightSpecular);

		// material
		shader->SetFloat3("material.ambient", material.ambient);
		shader->SetFloat3("material.diffuse", material.diffuse);
		shader->SetFloat3("material.specular", material.specular);
		shader->SetFloat("material.shininess", material.shininess);
		m_Texture->Bind(0);

		m_VertexArray->Bind();
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
