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
		tc.Translation = m_CameraPosition;
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.001f, 1000.0f);


		//sphere = CreateRef<Sphere>(glm::vec3(-3.0f, 0.0f, 3.0f));

		//light
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			m_LightShader = Shader::Create("Asset/shaders/glsl/Blinn-phong.glsl");
			m_FlatShader = Shader::Create("Asset/shaders/glsl/FlatShader.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			m_LightShader = Shader::Create("CubeShader", "Asset/shaders/hlsl/Solid_VS.hlsl", "Asset/shaders/hlsl/BlinnAndPhong.hlsl");
			m_FlatShader = Shader::Create("CubeShader", "Asset/shaders/hlsl/PosNormTcFrag_TransInvTrans.hlsl", "Asset/shaders/hlsl/flatcolor_PS.hlsl");

		}

		//material.SetTexutre("Asset/textures/matrix.jpg", Material::MaterialType::Emission);

		Material woodFloor = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		Material grass = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		Material window = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		// wood Texture
		Ref<Texture2D> woodTexture = TextureLibrary::Load("Asset/textures/floor.png");
		// grass Texture
		Ref<Texture2D> grassTexture = TextureLibrary::Load("Asset/textures/grass.png"); 
		// window Texture
		Ref<Texture2D> windowTexture = TextureLibrary::Load("Asset/textures/blending_transparent_window.png");
		woodFloor.SetTexture(woodTexture, Material::MaterialType::Diffuse);
		grass.SetTexture(grassTexture, Material::MaterialType::Diffuse);
		window.SetTexture(windowTexture, Material::MaterialType::Diffuse);

		m_LightShader->Bind();
		m_LightShader->SetInt("material.diffuse", 0);
		m_LightShader->SetInt("material.specular", 1);

		face = CreateRef<Face>(glm::vec3(0.0f, -3.0f, 0.0f),m_LightShader,woodFloor,5);
		sphere = CreateRef<Sphere>(glm::vec3(-0.1f, -2.6f, -1.0f), m_FlatShader, 0.1f);
		cube = CreateRef<Cube>(glm::vec3(-1.6f, -2.6f, -0.6f), m_LightShader);
		cube->SetScale({ 0.5f,0.5f,0.5f });
		cube2 = CreateRef<Cube>(glm::vec3(-0.8f, -2.6f, 0.0f), m_LightShader);
		cube2->SetScale({ 0.5f,0.5f,0.5f });

		grass1 = CreateRef<Face>(glm::vec3(0.0f, -2.75f, 0.0f), m_LightShader, grass, 1);
		grass1->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		grass1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		grass2 = CreateRef<Face>(glm::vec3(0.8f, -2.75f, 0.4f), m_LightShader, grass, 1);
		grass2->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		grass2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		grass3 = CreateRef<Face>(glm::vec3(1.0f, -2.75f, -0.5f), m_LightShader, grass, 1);
		grass3->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		grass3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		window1 = CreateRef<Face>(glm::vec3(0.9f, -2.75f, -0.7f), m_LightShader, window, 1);
		window1->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		window1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		window2 = CreateRef<Face>(glm::vec3(0.3f, -2.75f, 0.3f), m_LightShader, window, 1);
		window2->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		window2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		window3 = CreateRef<Face>(glm::vec3(0.5f, -2.75f, -0.1f), m_LightShader, window, 1);
		window3->SetRotation(glm::vec3(-1.6f, 0.0f, 0.0f));
		window3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
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
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;
			m_LightShader->Bind();
			m_LightShader->SetMat4("u_ViewProjection", camProj * viewMatrix);
			m_LightShader->SetFloat3("viewPosition", tc);
			m_LightShader->SetBool("blinn", blinn);
			m_LightShader->SetBool("gamma", gamma);

			// Point Light 1
			m_LightShader->SetFloat3("pointLight.position", sphere->GetTranslation());
			m_LightShader->SetFloat3("pointLight.ambient", glm::vec3(1.0f, 0.6f, 0.0f)*0.1f);
			m_LightShader->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
			m_LightShader->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

			m_LightShader->SetFloat("pointLight.constant", constant);
			m_LightShader->SetFloat("pointLight.Linear", Linear);
			m_LightShader->SetFloat("pointLight.quadratic", quadratic);
			face->Draw(m_LightShader);

			grass1->Draw(m_LightShader);
			grass2->Draw(m_LightShader);
			grass3->Draw(m_LightShader);

			window2->Draw(m_LightShader);
			window3->Draw(m_LightShader);
			window1->Draw(m_LightShader);

			RenderCommand::SetStencilTest(true);
			RenderCommand::SetStencilFunc(COMPARISON_FUNC::ALWAYS, 1);
			RenderCommand::SetStencilWriteMask(0xFF);
			cube->SetScale({ 0.5f, 0.5f, 0.5f });
			cube2->SetScale({ 0.5f, 0.5f, 0.5f });
			cube->Draw(m_LightShader);
			cube2->Draw(m_LightShader);
			m_LightShader->UnBind();

			m_FlatShader->Bind();

			m_FlatShader->SetMat4("u_ViewProjection", camProj * viewMatrix);
			m_FlatShader->SetFloat3("viewPosition", tc);
			
			RenderCommand::SetStencilFunc(COMPARISON_FUNC::NOT_EQUAL, 1);
			RenderCommand::SetStencilWriteMask(0x00);
			RenderCommand::SetDepthTest(false);
			m_FlatShader->SetFloat4("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			cube->SetScale({ 0.55f, 0.55f, 0.55f });
			cube2->SetScale({ 0.55f, 0.55f, 0.55f });
			cube->Draw(m_FlatShader);
			cube2->Draw(m_FlatShader);
			RenderCommand::SetDepthTest(true);
			RenderCommand::SetStencilWriteMask(0xFF);
			RenderCommand::SetStencilFunc(COMPARISON_FUNC::ALWAYS, 0);
			m_FlatShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			sphere->Draw(m_FlatShader);

			m_FlatShader->UnBind();
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
		ImGui::Checkbox("blinn", &blinn);
		ImGui::Checkbox("gamaa Corretion", &gamma);
		ImGui::End();

		face->ImguiRender("face 1");
		sphere->ImguiRender("Spehere 1");
		cube->ImguiRender("Cube1");
		grass1->ImguiRender("Grass 1");
		window1->ImguiRender("window1");
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
		static bool fpsmode = false;
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

				glm::vec3 front = { 0.0f,0.0f,1.0f };
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
