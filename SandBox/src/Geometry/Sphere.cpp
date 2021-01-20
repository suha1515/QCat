#include "Sphere.h"
#include <Imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>

namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Noemral;
		glm::vec2 TexCoord;
	};

	Sphere::Sphere(Scene* activeScene, const char* name, const glm::vec3& position, const char* texturePath, float radius, int sectorCount, int stackCount)
	{
		entity = activeScene->CreateEntity(name);
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Translation = position;

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = Shader::Create("Asset/shaders/glsl/Sphere.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = Shader::Create("SphereShader", "Asset/shaders/hlsl/Sphere_VS.hlsl", "Asset/shaders/hlsl/Sphere_PS.hlsl");
		}
		if (texturePath != "")
			m_Texture = QCat::Texture2D::Create(texturePath);

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
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = scale;
	}

	void Sphere::SetRotation(const glm::vec3& rotation)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Rotation = rotation;
	}

	void Sphere::Draw(Entity& camera)
	{
		shader->Bind();
		auto& transform = camera.GetComponent<TransformComponent>().GetTransform();
		auto& viewProj = camera.GetComponent<CameraComponent>().Camera.GetProjection() * glm::inverse(transform);
		shader->SetMat4("u_ViewProjection", viewProj);
		shader->SetMat4("u_Transform", entity.GetComponent<TransformComponent>().GetTransform());

		m_Texture->Bind(0);

		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Sphere::ImguiRender(const char* name)
	{
		ImGui::Begin(name);

		auto& tc = entity.GetComponent<TransformComponent>();
		ImGui::DragFloat3("Position", glm::value_ptr(tc.Translation), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(tc.Rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(tc.Scale), 0.1f);

		ImGui::End();
	}


}
