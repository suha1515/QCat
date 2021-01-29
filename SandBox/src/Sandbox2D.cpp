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


		sphere = CreateRef<Sphere>(glm::vec3(-3.0f, 0.0f, 3.0f));
		light = CreateRef<Light>(glm::vec3(2.0f, 0.0f, 5.0f));
		
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
		cameraSpeed = 2.5f*ts;
		auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;
		glm::mat4 rotationMat = glm::mat4(1.0f);
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

				rotationMat = glm::eulerAngleXYZ(glm::radians(pitch), glm::radians(yaw), 0.0f);
				glm::vec4 front = { 0.0f,0.0f,1.0f,0.0f };
				cameraFront = glm::vec3(rotationMat * front);
				cameraFront = glm::normalize(cameraFront);


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
		light->SetDirection(cameraFront);
		light->SetPosition(tc);
		cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront));
		cameraUp = glm::cross(cameraFront, cameraRight);

		viewMatrix = glm::lookAt(tc, tc + cameraFront, cameraUp);

		//m_Camera.GetComponent<TransformComponent>().Rotation = glm::vec3(-pitch, yaw, 0.0f);

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
			glm::mat4 camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			glm::mat4 cam = viewMatrix;//m_Camera.GetComponent<TransformComponent>().GetTransform();
			cube->Draw(cam, camProj,light->Getinfo());
			cube1->Draw(cam, camProj, light->Getinfo());
			cube2->Draw(cam, camProj, light->Getinfo());
			cube3->Draw(cam, camProj, light->Getinfo());
			cube4->Draw(cam, camProj, light->Getinfo());
			cube5->Draw(cam, camProj, light->Getinfo());
			sphere->Draw(cam, camProj,light->Getinfo());
			light->Draw(cam, camProj);
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
		sphere->ImguiRender("Spehere 1");
		light->ImGuiRender("light 1");
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
