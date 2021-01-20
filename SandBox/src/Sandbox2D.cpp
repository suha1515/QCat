#include "Sandbox2D.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>

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
		m_Cube = m_ActiveScene->CreateEntity("Cube");
		
		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = glm::vec3(0.0f, 0.0f, -5.0f);
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.001f, 1000.0f);

		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 3.0f), "Asset/textures/Checkerboard.png");
		sphere = CreateRef<Sphere>(glm::vec3(-3.0f, 0.0f, 3.0f), "Asset/textures/Checkerboard.png");
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
			glm::mat4& viewProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection() * glm::inverse(m_Camera.GetComponent<TransformComponent>().GetTransform());
			cube->Draw(viewProj,light->GetColor());
			sphere->Draw(viewProj, light->GetColor());
			light->Draw(viewProj);
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
		SceneCamera* sceneCamera = &m_Camera.GetComponent<CameraComponent>().Camera;
		auto fov = sceneCamera->GetPerspectiveVerticalFov();
		if (ImGui::DragFloat("Camera FOv", &fov, 0.1f))
		{
			sceneCamera->SetPerspectiveVerticalFov(fov);
		}
		
		ImGui::DragFloat3("Cube Position", glm::value_ptr(transformCube.Translation), 0.1f);
		ImGui::DragFloat3("Cube Rotation", glm::value_ptr(transformCube.Rotation), 0.1f);
		ImGui::DragFloat3("Cube Scale", glm::value_ptr(transformCube.Scale), 0.1f);

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
