#include "Cube.h"
#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float tilingFactor;
	};
	Cube::Cube(Scene* activeScene , const char* name,const glm::vec3& position, const char* texturePath)
	{
		entity = activeScene->CreateEntity(name);
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Translation = position;
		
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = Shader::Create("Asset/shaders/glsl/Solid.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = Shader::Create("CubeShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/Solid_PS.hlsl");
		}
		
		if(texturePath != "")
			m_Texture = QCat::Texture2D::Create(texturePath);

		shader->Bind();
		shader->SetInt("u_Texture", 0);

		// VertexArray
		m_VertexArray = VertexArray::Create();

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(24 * sizeof(Vertex));

		Vertex vertex[24];
		vertex[0] = { {-0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[1] = { { 0.5f,-0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[2] = { { 0.5f, 0.5f,-0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[3] = { {-0.5f, 0.5f,-0.5f},{1.0f,0.0f,1.0f,1.0f},{0.0f,1.0f},1.0f };

		vertex[4] = { { 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[5] = { { 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[6] = { { 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[7] = { { 0.5f, 0.5f,-0.5f},{1.0f,1.0f,0.0f,1.0f},{0.0f,1.0f},1.0f };

		vertex[8] = { { 0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[9] = { {-0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[10] = { {-0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[11] = { { 0.5f, 0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f},{0.0f,1.0f},1.0f };

		vertex[12] = { {-0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[13] = { {-0.5f,-0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[14] = { {-0.5f, 0.5f,-0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[15] = { {-0.5f, 0.5f, 0.5f},{1.0f,1.0f,0.0f,1.0f},{0.0f,1.0f},1.0f };

		vertex[16] = { {-0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[17] = { { 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[18] = { { 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[19] = { {-0.5f, 0.5f, 0.5f},{1.0f,0.0f,1.0f,1.0f},{0.0f,1.0f},1.0f };

		vertex[20] = { {-0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[21] = { { 0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[22] = { { 0.5f,-0.5f,-0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[23] = { {-0.5f,-0.5f,-0.5f},{1.0f,1.0f,0.0f,1.0f},{0.0f,1.0f},1.0f };


		vertexBuffer->SetData(vertex, sizeof(vertex));

		//IndexBuffer
		uint32_t Indices[36] = {
			0,1,2, 2,3,0,
			4,5,6, 6,7,4,
			8,9,10, 10,11,8,
			12,13,14,14,15,12,
			16,17,18,18,19,16,
			20,21,22,22,23,20
		};

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));


		vertexBuffer->SetLayout(BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position"},
			  { ShaderDataType::Float4, "a_Color"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			  { ShaderDataType::Float,  "a_TilingFactor"}
			}, shader
		));

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

	}
	void Cube::SetScale(const glm::vec3& scale)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Scale = scale;
	}
	void Cube::SetRotation(const glm::vec3& rotation)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		tc.Rotation= rotation;
	}
	void Cube::Draw(Entity& camera)
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
	void Cube::ImguiRender(const char* name)
	{
		ImGui::Begin(name);
		
		auto& tc = entity.GetComponent<TransformComponent>();
		ImGui::DragFloat3("Position", glm::value_ptr(tc.Translation), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(tc.Rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(tc.Scale), 0.1f);
		
		ImGui::End();
	}
}