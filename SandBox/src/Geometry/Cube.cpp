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
	};
	Cube::Cube(const glm::vec3& position)
		:translation(position),rotation(glm::vec3(0.0f,0.0f,0.0f)),scale(glm::vec3(1.0f,1.0f,1.0f))
		,material(glm::vec3(1.0f,0.5f,0.31f),glm::vec3(1.0f,0.5f,0.31f),glm::vec3(0.5f,0.5f,0.5f),32.0f)
	{

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			shader = Shader::Create("Asset/shaders/glsl/Spotlight.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			shader = Shader::Create("CubeShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/SpotLight_PS.hlsl");
		}
		
		material.SetTexutre("Asset/textures/container2.png", Material::MaterialType::Diffuse);
		material.SetTexutre("Asset/textures/container2_specular.png", Material::MaterialType::Specular);
		//material.SetTexutre("Asset/textures/matrix.jpg", Material::MaterialType::Emission);

		shader->Bind();
		shader->SetInt("material.diffuse", 0);
		shader->SetInt("material.specular",1);
		//shader->SetInt("material.emission",2);

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
	void Cube::Draw(const glm::mat4& view, const glm::mat4& proj, LightInfo info)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", proj* view);
		glm::mat4 transform= glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
		glm::vec3 viewpos = -1.0f * view[3];
		shader->SetMat4("u_Transform", transform);
		shader->SetMat4("u_invTransform", glm::inverse(transform));
		shader->SetFloat3("viewPosition", viewpos);

		// light
		shader->SetFloat3("light.diffuse", info.lightColor);
		shader->SetFloat3("light.ambient", info.lightAmbient);
		shader->SetFloat3("light.position", info.lightPos);
		shader->SetFloat3("light.specular", info.lightSpecular);
		shader->SetFloat3("light.direction", info.lightDirection);

		// pointer light
		shader->SetFloat("light.constant", info.constant);
		shader->SetFloat("light.Linear", info.linear);
		shader->SetFloat("light.quadratic", info.quadratic);
		shader->SetFloat("light.cutOff", info.cutoff);
		shader->SetFloat("light.outerCutOff", info.outerCutOff);

		// material
		shader->SetFloat3("material.specular", material.specular);
		shader->SetFloat("material.shininess", material.shininess);

	

		material.Bind();

		m_VertexArray->Bind();
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