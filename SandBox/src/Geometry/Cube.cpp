#include "Cube.h"
#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "../Light/Light.h"
namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Noemral;
		glm::vec2 TexCoord;
	};
	Cube::Cube(const glm::vec3& position, const char* texturePath)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f))
	{

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = Shader::Create("Asset/shaders/glsl/FlatShader.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = Shader::Create("CubeShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/Solid_PS.hlsl");
		}
		
		if(texturePath != "")
			m_Texture = QCat::Texture2D::Create(texturePath);
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

		// VertexBuffer
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(24 * sizeof(Vertex));

		Vertex vertex[24];
		vertex[0] = { {-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{0.0f,0.0f}};
		vertex[1] = { { 0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,0.0f}};
		vertex[2] = { { 0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,1.0f}};
		vertex[3] = { {-0.5f, 0.5f,-0.5f},{0.0f,0.0f,-1.0f},{0.0f,1.0f}};

		vertex[4] = { { 0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,0.0f}};
		vertex[5] = { { 0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f},{1.0f,0.0f}};
		vertex[6] = { { 0.5f, 0.5f, 0.5f},{1.0f,0.0f,0.0f},{1.0f,1.0f}};
		vertex[7] = { { 0.5f, 0.5f,-0.5f},{1.0f,0.0f,0.0f},{0.0f,1.0f}};

		vertex[8] = { { 0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0f,0.0f}};
		vertex[9] = { {-0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f},{1.0f,0.0f}};
		vertex[10]= { {-0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{1.0f,1.0f}};
		vertex[11]= { { 0.5f, 0.5f, 0.5f},{0.0f,0.0f,1.0f},{0.0f,1.0f}};

		vertex[12]= { {-0.5f,-0.5f, 0.5f},{-1.0f,0.0f,0.0f},{0.0f,0.0f}};
		vertex[13]= { {-0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,0.0f}};
		vertex[14]= { {-0.5f, 0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,1.0f}};
		vertex[15]= { {-0.5f, 0.5f, 0.5f},{-1.0f,0.0f,0.0f},{0.0f,1.0f}};
				  
		vertex[16]= { {-0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{0.0f,0.0f}};
		vertex[17]= { { 0.5f, 0.5f,-0.5f},{0.0f,1.0f,0.0f},{1.0f,0.0f}};
		vertex[18]= { { 0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f},{1.0f,1.0f}};
		vertex[19]= { {-0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f},{0.0f,1.0f}};
				  
		vertex[20]= { {-0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{0.0f,0.0f}};
		vertex[21]= { { 0.5f,-0.5f, 0.5f},{0.0f,-1.0f,0.0f},{1.0f,0.0f}};
		vertex[22]= { { 0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{1.0f,1.0f}};
		vertex[23]= { {-0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{0.0f,1.0f}};


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
			  { ShaderDataType::Float3, "a_Normal"   },
			  { ShaderDataType::Float2, "a_TexCoord"},
			}, shader
		));

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
	void Cube::Draw(const glm::mat4& cameraTransform, const glm::mat4& proj, LightInfo info)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", proj*glm::inverse(cameraTransform));
		glm::mat4 transform= glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		shader->SetMat4("u_Transform", transform);
		shader->SetFloat3("lightColor", info.lightColor);
		shader->SetFloat3("lightAmbient", info.lightAmbient);
		shader->SetFloat3("lightPos", info.lightPos);
		glm::vec3 viewpos = cameraTransform[3];
		shader->SetFloat3("viewPos", viewpos);
		shader->SetFloat("specularstrength", info.specularStrength);
		shader->SetInt("shininess", info.shininess);

		m_Texture->Bind(0);

		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray);
	}
	void Cube::ImguiRender(const char* name)
	{
		ImGui::Begin(name);
		
		ImGui::DragFloat3("Position", glm::value_ptr(translation), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);
		
		ImGui::End();
	}
}