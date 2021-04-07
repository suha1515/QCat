#include "../Header/PbrTest.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace QCat
{
	PbrTest::PbrTest()
		:Layer("PbrTest")
	{
	}

	void PbrTest::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");

		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 0.0f,0.0f,-5.0f };
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.01f, 100.0f);

		//PBRshader = ShaderLibrary::Load("Blinn-Phong", "Asset/shaders/glsl/VertexShader/World_TexNormFragTbn_Out.glsl",
		//	"Asset/shaders/glsl/PixelShader/Blinn-phong.glsl");
		Flatcolor = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/FlatColor.hlsl" : "Asset/shaders/glsl/FlatColor.glsl");
		PBRshader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBR_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBR_PointLight.glsl");

		PBRshader->Bind();
		PBRshader->SetInt("material.albedoMap", 0, ShaderType::PS);
		PBRshader->SetInt("material.normalMap", 1, ShaderType::PS);
		PBRshader->SetInt("material.metallicMap", 2, ShaderType::PS);
		PBRshader->SetInt("material.roughnessMap", 3, ShaderType::PS);
		PBRshader->SetInt("material.aoMap", 4, ShaderType::PS);


		Sampler_Desc desc;
		pbrmat.SetTexture("Asset/textures/rusted_iron/albedo.png", desc, Material::TextureType::Diffuse);
		pbrmat.SetTexture("Asset/textures/rusted_iron/normal.png", desc, Material::TextureType::Normal);
		pbrmat.SetTexture("Asset/textures/rusted_iron/metallic.png", desc, Material::TextureType::Metallic);
		pbrmat.SetTexture("Asset/textures/rusted_iron/roughness.png", desc, Material::TextureType::Roughness);

		//Brick.m_DiffuseTexture = brick;
		//Brick.m_NormalMapTexture = brick_normal;

		lightPosition = glm::vec3(10.0f, 10.0f, -20.0f);
		light[0].info.lightPosition = lightPosition;
		light[1].info.lightPosition = { -10.0f,10.0f,-20.0f};
		light[2].info.lightPosition = {  10.0f,-10.0f,-20.0f };
		light[3].info.lightPosition = { -10.0f,-10.0f,-20.0f };

		light[0].info.diffuse = { 300.0f,300.0f,300.0f };
		light[1].info.diffuse = { 300.0f,300.0f,300.0f };
		light[2].info.diffuse = { 300.0f,300.0f,300.0f };
		light[3].info.diffuse = { 300.0f,300.0f,300.0f };

		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
	}

	void PbrTest::OnDetach()
	{
	}

	void PbrTest::OnUpdate(Timestep ts)
	{
		QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		QCat::RenderCommand::Clear();

		RenderCommand::SetDepthTest(true);

		CameraUpdate(ts);
		const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
		auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;

		PBRshader->Bind();
		PBRshader->SetMat4("u_ViewProjection", camProj * viewMatrix,ShaderType::VS);
		PBRshader->SetFloat3("u_viewPosition", tc, ShaderType::VS);

		for (int i = 0; i < 4; ++i)
		{
			std::string lightname = "pointLight[" + std::to_string(i) + "].";
			PBRshader->SetFloat3(lightname +"position", light[i].info.lightPosition, ShaderType::PS);
			PBRshader->SetFloat3(lightname +"diffuse", light[i].info.diffuse, ShaderType::PS);
		}
		
		sphere->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < 7; ++j)
			{
				sphere->SetTranslation(glm::vec3(-1.05f+j*0.3f, 1.05f-i*0.3f, 0.0f));
				glm::mat4 transform = sphere->GetTransform();
				PBRshader->SetMat4("u_Transform", transform, ShaderType::VS);
				PBRshader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
				// material
				if (pbrmat.IsThereTexture(Material::TextureType::Normal))
					PBRshader->SetBool("material.IsNormalMap", true, ShaderType::PS);
				else
					PBRshader->SetBool("material.IsNormalMap", false, ShaderType::PS);
				PBRshader->UpdateBuffer();

				pbrmat.Bind(0,Material::TextureType::Diffuse);
				pbrmat.Bind(1, Material::TextureType::Normal);
				pbrmat.Bind(2, Material::TextureType::Metallic);
				pbrmat.Bind(3, Material::TextureType::Roughness);
				pbrmat.Bind(4, Material::TextureType::AmbientOcclusion);
				sphere->Draw(PBRshader);
			}
		}
		PBRshader->UnBind();

		Flatcolor->Bind();
		Flatcolor->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
		Flatcolor->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
		for (int i = 0; i < 4; ++i)
		{
			sphere->SetTranslation(light[i].info.lightPosition);
			sphere->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 transform = sphere->GetTransform();
			Flatcolor->SetMat4("u_Transform", transform, ShaderType::VS);
			Flatcolor->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
			Flatcolor->UpdateBuffer();
			sphere->Draw(Flatcolor);
		}
		
		Flatcolor->UnBind();

	}

	void PbrTest::OnImGuiRender()
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
		ImGui::DragFloat3("cameraFront", glm::value_ptr(cameraFront), 0.1f);
		ImGui::DragFloat3("lightPos", glm::value_ptr(light[0].info.lightPosition), 0.1f);
		ImGui::End();
	}

	void PbrTest::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
	}

	bool PbrTest::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}
	void PbrTest::CameraUpdate(QCat::Timestep ts)
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

