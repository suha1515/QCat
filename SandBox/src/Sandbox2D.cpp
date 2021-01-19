#include "Sandbox2D.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>

namespace QCat
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float tilingFactor;
	};
	Sandbox2D::Sandbox2D()
		:Layer("Sandbox2D")
	{
	}

	void Sandbox2D::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");
		m_Cube = m_ActiveScene->CreateEntity("Cube");

		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = glm::vec3(0.0f, 0.0f, -2.0f);
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.001f, 1000.0f);

	/*	m_Projection = glm::perspective(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
		m_CameraPosition = { 0.0f,0.0f,-2.0f };
		auto camTransmform = glm::translate(glm::mat4(1.0f), m_CameraPosition) * 
			 glm::rotate(glm::mat4(1.0f),m_CameraRotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
		    *glm::rotate(glm::mat4(1.0f), m_CameraRotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			*glm::rotate(glm::mat4(1.0f), m_CameraRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		m_View = glm::inverse(camTransmform);*/

		m_MainCamera = &camera.Camera;
		m_Texture = QCat::Texture2D::Create("Asset/textures/Checkerboard.png");

		shader = Shader::Create("Asset/shaders/glsl/Solid.glsl");
		shader->Bind();
		shader->SetInt("u_Texture", 0);

		// VertexArray
		m_VertexArray = VertexArray::Create();

		

		//float vertex[3 * 4] =
		//{
		//	-0.5f,-0.5f,0.0f,
		//	 0.5f,-0.5f,0.0f,
		//	 0.5f, 0.5f,0.0f,
		//	-0.5f, 0.5f,0.0f
		//};
		//Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertex, sizeof(vertex));

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

		vertex[8] =  { { 0.5f,-0.5f, 0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[9] =  { {-0.5f,-0.5f, 0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
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

		vertex[20] = { {-0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f},1.0f };
		vertex[21] = { { 0.5f,-0.5f,-0.5f},{0.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},1.0f };
		vertex[22] = { { 0.5f,-0.5f, 0.5f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f},1.0f };
		vertex[23] = { {-0.5f,-0.5f, 0.5f},{1.0f,1.0f,0.0f,1.0f},{0.0f,1.0f},1.0f };
		

		vertexBuffer->SetData(vertex,sizeof(vertex));
		
		//IndexBuffer
	/*	uint32_t Indices[6] = {
			0,1,2, 2,3,0
		};*/
		uint32_t Indices[36] = {
			0,1,2, 2,3,0,
			4,5,6, 6,7,4,
			8,9,10, 10,11,8,
			12,13,14,14,15,12,
			16,17,18,18,19,16,
			20,21,22,22,23,20
		};

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(Indices,sizeof(Indices)/sizeof(uint32_t));

	/*	vertexBuffer->SetLayout(BufferLayout::Create(
			  { { ShaderDataType::Float3, "a_Position"}
			   },shader
		));*/

		vertexBuffer->SetLayout(BufferLayout::Create(
			  { { ShaderDataType::Float3, "a_Position"},
			    { ShaderDataType::Float4, "a_Color"   },
			    { ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float,  "a_TilingFactor"}
			   },shader 
		));

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);
#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
	}

	void Sandbox2D::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void Sandbox2D::OnUpdate(QCat::Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		// Update
		/*auto camTransmform = glm::translate(glm::mat4(1.0f), m_CameraPosition) *
			glm::rotate(glm::mat4(1.0f), m_CameraRotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), m_CameraRotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), m_CameraRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		m_View = glm::inverse(camTransmform);*/
		// Render
		// Reset stats here
		QCat::Renderer2D::ResetStats();
		{
			QCAT_PROFILE_SCOPE("Renderer Prep");
			QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			QCat::RenderCommand::Clear();
		}
		{
			QCAT_PROFILE_SCOPE("Renderer Draw");

			shader->Bind();
			//glm::mat4 viewProj = m_Projection * m_View;
			auto transform = m_Camera.GetComponent<TransformComponent>().GetTransform();
			auto viewProj = m_MainCamera->GetProjection() * glm::inverse(transform);
			shader->SetMat4("u_ViewProjection", viewProj);
			shader->SetMat4("u_Transform", m_Cube.GetComponent<TransformComponent>().GetTransform());
			//shader->SetMat4("u_Transform", glm::mat4(1.0f));
			
			//shader->SetMat4("u_Transform", glm::mat4(1.0f));
			//shader->SetFloat4("u_Color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
			m_Texture->Bind(0);

			m_VertexArray->Bind();
			RenderCommand::DrawIndexed(m_VertexArray);
		}
	}

	void Sandbox2D::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");
		auto& transformCamera = m_Camera.GetComponent<TransformComponent>();
		auto& transformCube = m_Cube.GetComponent<TransformComponent>();

		if (ImGui::DragFloat3("CameraPosition", glm::value_ptr(transformCamera.Translation), 0.1f))
		{

		}
		if (ImGui::DragFloat3("CameraRotation", glm::value_ptr(transformCamera.Rotation), 0.1f))
		{

		}
		auto fov = m_MainCamera->GetPerspectiveVerticalFov();
		if (ImGui::DragFloat("Camera FOv", &fov, 0.1f))
		{
			m_MainCamera->SetPerspectiveVerticalFov(fov);
		}
		
		ImGui::DragFloat3("Cube Position", glm::value_ptr(transformCube.Translation), 0.1f);
		ImGui::DragFloat3("Cube Rotation", glm::value_ptr(transformCube.Rotation), 0.1f);
		ImGui::DragFloat3("Cube Scale", glm::value_ptr(transformCube.Scale), 0.1f);

		ImGui::End();
	}

	void Sandbox2D::OnEvent(QCat::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));


	}

	bool Sandbox2D::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			return false;
		}
		m_Camera.GetComponent<CameraComponent>().Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		return false;
	}

}
