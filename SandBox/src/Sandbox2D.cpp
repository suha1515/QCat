#include "Sandbox2D.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>
#include <QCat/InputDevice/Mouse/Mouse.h>
namespace QCat
{

	Sandbox2D::Sandbox2D()
		:Layer("Sandbox2D")
	{
	}

	void Sandbox2D::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");
		
		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = glm::vec3(0.0f, 0.0f, -5.0f);
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.001f, 1000.0f);

		cube = CreateRef<Cube>(glm::vec3(0.0f, -1.6f, 4.5f));

		cube1 = CreateRef<Cube>(glm::vec3(-2.1f, -1.4f, 5.6f));
		cube1->SetRotation({ -0.5,0.3,-0.8 });

		cube2 = CreateRef<Cube>(glm::vec3(-0.5f, -0.3f, 10.5f));
		cube2->SetRotation({ -1.3,-0.4,-1.3 });

		cube3 = CreateRef<Cube>(glm::vec3(1.9f, -0.9f, 3.6f));
		cube3->SetRotation({ -2.3,-0.4,-1.6 });

		cube4 = CreateRef<Cube>(glm::vec3(2.2f, 1.4f, 5.2f));
		cube4->SetRotation({ -2.4,-2.2,-0.5 });

		cube5 = CreateRef<Cube>(glm::vec3(-1.9f, 1.4f, 5.2f));
		cube5->SetRotation({ -3.1,-1.5,-0.5 });


		
		//sphere = CreateRef<Sphere>(glm::vec3(-3.0f, 0.0f, 3.0f));

		//light
	

		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			m_LightShader = Shader::Create("Asset/shaders/glsl/MultiLight.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			m_LightShader = Shader::Create("CubeShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/SpotLight_PS.hlsl");
		}

		cube->SetShader(m_LightShader);
		cube1->SetShader(m_LightShader);
		cube2->SetShader(m_LightShader);
		cube3->SetShader(m_LightShader);
		cube4->SetShader(m_LightShader);
		cube5->SetShader(m_LightShader);
		//material.SetTexutre("Asset/textures/matrix.jpg", Material::MaterialType::Emission);

		m_LightShader->Bind();
		m_LightShader->SetInt("material.diffuse", 0);
		m_LightShader->SetInt("material.specular", 1);
		//shader->SetInt("material.emission",2);

		
		//RenderCommand::SetWireFrameMode();
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
		CameraUpdate(ts);
		//Spotlight->SetDirection(cameraFront);
		//Spotlight->SetPosition(tc);

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
			const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			m_LightShader->Bind();
			m_LightShader->SetMat4("u_ViewProjection", camProj * viewMatrix);
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;
			m_LightShader->SetFloat3("viewPosition", tc);

			// Directional Light
			m_LightShader->SetFloat3("dirLight.direction", glm::vec3(-0.2f, -1.0f, 0.3f));
			m_LightShader->SetFloat3("dirLight.ambient", glm::vec3(0.1f, 0.14f, 0.14f));
			m_LightShader->SetFloat3("dirLight.diffuse", glm::vec3(0.1f, 0.12f, 0.16f));
			m_LightShader->SetFloat3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

			// Point Light 1
			m_LightShader->SetFloat3("pointLight[0].position", glm::vec3(0.7f, 0.2f, 3.0f));
			m_LightShader->SetFloat3("pointLight[0].ambient", glm::vec3(1.0f, 0.6f, 0.0f)*0.1f);
			m_LightShader->SetFloat3("pointLight[0].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
			m_LightShader->SetFloat3("pointLight[0].specular", glm::vec3(1.0f, 0.6f, 0.0f));

			m_LightShader->SetFloat("pointLight[0].constant", 1.0f);
			m_LightShader->SetFloat("pointLight[0].Linear", 0.09f);
			m_LightShader->SetFloat("pointLight[0].quadratic", 0.032f);

			// Point Light 2
			m_LightShader->SetFloat3("pointLight[1].position", glm::vec3(3.0f, -3.3f, -4.0f));
			m_LightShader->SetFloat3("pointLight[1].ambient", glm::vec3(1.0f, 0.0f, 0.0f)*0.1f);
			m_LightShader->SetFloat3("pointLight[1].diffuse", glm::vec3(0.0f, 1.0f, 0.0f));
			m_LightShader->SetFloat3("pointLight[1].specular", glm::vec3(1.0f, 0.0f, 0.0f));
												 
			m_LightShader->SetFloat ("pointLight[1].constant", 1.0f);
			m_LightShader->SetFloat ("pointLight[1].Linear", 0.09f);
			m_LightShader->SetFloat ("pointLight[1].quadratic", 0.032f);

			// Point Light 3
			m_LightShader->SetFloat3("pointLight[2].position", glm::vec3(-4.0f, 2.0f, -6.0f));
			m_LightShader->SetFloat3("pointLight[2].ambient", glm::vec3(1.0f, 1.0, 0.0) * 0.1f);
			m_LightShader->SetFloat3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0, 1.0));
			m_LightShader->SetFloat3("pointLight[2].specular", glm::vec3(1.0f, 1.0, 0.0));
												 
			m_LightShader->SetFloat ("pointLight[2].constant", 1.0f);
			m_LightShader->SetFloat ("pointLight[2].Linear", 0.09f);
			m_LightShader->SetFloat ("pointLight[2].quadratic", 0.032f);

			// Point Light 4
			m_LightShader->SetFloat3("pointLight[3].position", glm::vec3(0.0f, 0.0f, -3.0f));
			m_LightShader->SetFloat3("pointLight[3].ambient", glm::vec3(0.2f, 0.2f, 1.0f) * 0.1f);
			m_LightShader->SetFloat3("pointLight[3].diffuse", glm::vec3(0.2f, 0.2f, 1.0f));
			m_LightShader->SetFloat3("pointLight[3].specular", glm::vec3(0.2f, 0.2f, 1.0f));
												 
			m_LightShader->SetFloat ("pointLight[3].constant", 1.0f);
			m_LightShader->SetFloat ("pointLight[3].Linear", 0.09f);
			m_LightShader->SetFloat ("pointLight[3].quadratic", 0.032f);
		
			//spotLight
			m_LightShader->SetFloat3("spotlight.position", tc);
			m_LightShader->SetFloat3("spotlight.direction", cameraFront);
			m_LightShader->SetFloat3("spotlight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			m_LightShader->SetFloat3("spotlight.diffuse", glm::vec3(1.f, 1.f, 0.0f));
			m_LightShader->SetFloat3("spotlight.specular", glm::vec3(0.8f, 0.8f, 0.0f));

			m_LightShader->SetFloat ("spotlight.constant", 1.0f);
			m_LightShader->SetFloat ("spotlight.Linear", 0.09f);
			m_LightShader->SetFloat ("spotlight.quadratic", 0.032f);
			m_LightShader->SetFloat ("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
			m_LightShader->SetFloat ("spotlight.outerCutOff", glm::cos(glm::radians(13.0f)));


			cube->Draw(m_LightShader);
			cube1->Draw(m_LightShader);
			cube2->Draw(m_LightShader);
			cube3->Draw(m_LightShader);
			cube4->Draw(m_LightShader);
			cube5->Draw(m_LightShader);
			m_LightShader->UnBind();
			//sphere->Draw(m_LightShader);
			//light->Draw(m_LightShader);
		}
	}

	void Sandbox2D::OnImGuiRender()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");
		auto& transformCamera = m_Camera.GetComponent<TransformComponent>();

		if (ImGui::DragFloat3("CameraPosition", glm::value_ptr(transformCamera.Translation), 0.1f))
		{

		}
		if (ImGui::DragFloat3("CameraRotation", glm::value_ptr(transformCamera.Rotation), 0.1f))
		{

		}
		SceneCamera* sceneCamera = &m_Camera.GetComponent<CameraComponent>().Camera;
		auto fov = sceneCamera->GetPerspectiveVerticalFov();
		if (ImGui::DragFloat("Camera FOv", &fov, 0.1f))
		{
			sceneCamera->SetPerspectiveVerticalFov(fov);
		}
		ImGui::DragFloat3("cameraFront", glm::value_ptr(cameraFront),0.1f);

		ImGui::End();

		cube->ImguiRender("Cube 1");
		//sphere->ImguiRender("Spehere 1");
		//light->ImGuiRender("light 1");
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

	void Sandbox2D::CameraUpdate(QCat::Timestep ts)
	{
		cameraSpeed = 2.5f * ts;
		auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;
		// camera roration by mouse
		static bool fpsmode = true;
		if (fpsmode)
		{
			while (const auto delta = Input::GetDeltaData())
			{
				float dx = (float)delta->x;
				float dy = (float)delta->y;

				yaw -= dx * 0.04f;
				pitch -= dy * 0.04f;
				//QCAT_INFO("dx : {0} , dy{1}", dx, dy);

				if (pitch > 89.0f)
					pitch = 89.f;
				if (pitch < -89.0f)
					pitch = -89.f;

				glm::vec4 front = { 0.0f,0.0f,1.0f,0.0f };
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);
			}
			// camera Move
			if (Input::IsKeyPressed(Key::W))
			{
				tc += cameraSpeed * cameraFront;
			}
			if (Input::IsKeyPressed(Key::S))
			{
				tc -= cameraSpeed * cameraFront;
			}
			if (Input::IsKeyPressed(Key::A))
			{
				tc -= cameraSpeed * cameraRight;
			}
			if (Input::IsKeyPressed(Key::D))
			{
				tc += cameraSpeed * cameraRight;
			}
			if (Input::IsKeyPressed(Key::Escape))
				fpsmode = false;
		}
		else
		{
			if (Input::IsKeyPressed(Key::Escape))
				fpsmode = true;
		}

		cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront));
		cameraUp = glm::cross(cameraFront, cameraRight);

		viewMatrix = glm::lookAt(tc, tc + cameraFront, cameraUp);
	}

}
