#include "TestScene.h"

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
	TestScene::TestScene()
		:Layer("TestScene")
	{
	}

	void TestScene::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");
		
		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 0.0f,0.0f,-5.0f };
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.01f, 100.0f);

		//Shader
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			BlinnPhongShader = ShaderLibrary::Load("Asset/shaders/glsl/Blinn-phong.glsl");
			BlinnPhongParallax = ShaderLibrary::Load("Asset/shaders/glsl/ParrallaxMapping/Blinn-phong-Parrallax.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			BlinnPhongShader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl", "Asset/shaders/hlsl/BlinnAndPhong_PS.hlsl" );
			BlinnPhongParallax = ShaderLibrary::Load("parallax","Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl",
												"Asset/shaders/hlsl/ParallaxMapping/BlinnAndPhongParallax_PS.hlsl");

		}

		BlinnPhongShader->Bind();
		BlinnPhongShader->SetInt("material.diffuse", 0, ShaderType::PS);
		BlinnPhongShader->SetInt("material.specular", 1, ShaderType::PS);
		BlinnPhongShader->SetInt("material.normal", 2, ShaderType::PS);

		BlinnPhongParallax->Bind();
		BlinnPhongParallax->SetInt("material.diffuse", 0, ShaderType::PS);
		BlinnPhongParallax->SetInt("material.specular", 1, ShaderType::PS);
		BlinnPhongParallax->SetInt("material.normal", 2, ShaderType::PS);
		BlinnPhongParallax->SetInt("heightMap", 4, ShaderType::PS);

		// Brickwall Texture
		Ref<Texture2D> brickTexture = TextureLibrary::Load("Asset/textures/brickwall.jpg");
		// Brickwall normalmap
		Ref<Texture2D> brickNormalMap = TextureLibrary::Load("Asset/textures/brickwall_normal.jpg");

		// Brickwall2 Texture
		Ref<Texture2D> brickTexture2 = TextureLibrary::Load("Asset/textures/bricks2.jpg");
		// Brickwall2 normalmap
		Ref<Texture2D> brickNormalMap2 = TextureLibrary::Load("Asset/textures/bricks2_normal.jpg");

		heightMap = TextureLibrary::Load("Asset/textures/bricks2_disp.jpg");

		brick.SetTexture(brickTexture, Material::MaterialType::Diffuse);
		brick.SetTexture(brickNormalMap, Material::MaterialType::NormalMap);

		brick2.SetTexture(brickTexture2, Material::MaterialType::Diffuse);
		brick2.SetTexture(brickNormalMap2, Material::MaterialType::NormalMap);


		face = CreateRef<Face>(glm::vec3(0.0f, 0.0f, 0.0f), BlinnPhongShader, brick, 1);
		LightPosition = glm::vec3(0.0f, 0.0f, -2.0f);
	}

	void TestScene::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

	}
	void TestScene::OnUpdate(QCat::Timestep ts)
	{
		QCAT_PROFILE_FUNCTION();
		// Update
		CameraUpdate(ts);
	
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

			BlinnPhongShader->Bind();
			BlinnPhongShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			BlinnPhongShader->SetFloat3("viewPosition", tc, ShaderType::VS);
			// Point Light 
			BlinnPhongShader->SetFloat3("pointLight.position", LightPosition, ShaderType::PS);
			BlinnPhongShader->SetFloat3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f), ShaderType::PS);
			BlinnPhongShader->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
			BlinnPhongShader->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);

			BlinnPhongShader->SetFloat("pointLight.constant", constant, ShaderType::PS);
			BlinnPhongShader->SetFloat("pointLight.Linear", Linear, ShaderType::PS);
			BlinnPhongShader->SetFloat("pointLight.quadratic", quadratic, ShaderType::PS);

			face->SetTranslation({ -1.0f,0.0f,0.0f });
			face->SetMaterial(brick2);
			face->MaterialBind();
			face->Draw(BlinnPhongShader);
			BlinnPhongShader->UnBind();

			BlinnPhongParallax->Bind();
			BlinnPhongParallax->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			BlinnPhongParallax->SetFloat3("viewPosition", tc, ShaderType::VS);
			BlinnPhongParallax->SetFloat("height_scale",height, ShaderType::PS);
			// Point Light 
			BlinnPhongParallax->SetFloat3("pointLight.position", LightPosition, ShaderType::PS);
			BlinnPhongParallax->SetFloat3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f), ShaderType::PS);
			BlinnPhongParallax->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
			BlinnPhongParallax->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);

			BlinnPhongParallax->SetFloat("pointLight.constant", constant, ShaderType::PS);
			BlinnPhongParallax->SetFloat("pointLight.Linear", Linear, ShaderType::PS);
			BlinnPhongParallax->SetFloat("pointLight.quadratic", quadratic, ShaderType::PS);

			face->SetTranslation({ 1.0f,0.0f,0.0f });
			face->SetMaterial(brick2);
			face->MaterialBind();
			heightMap->Bind(4);
			face->Draw(BlinnPhongParallax);
			BlinnPhongParallax->UnBind();
		}
	}

	void TestScene::OnImGuiRender()
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
		ImGui::DragFloat3("lightPosition", glm::value_ptr(LightPosition), 0.1f);
		ImGui::DragFloat("Height", &height, 0.01f);
		
		ImGui::End();
	}

	void TestScene::OnEvent(QCat::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

	}

	bool TestScene::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			return false;
		}
		m_Camera.GetComponent<CameraComponent>().Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		return false;
	}

	void TestScene::CameraUpdate(QCat::Timestep ts)
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
