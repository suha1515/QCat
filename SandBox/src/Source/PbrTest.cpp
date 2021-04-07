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
		PBRshader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBRColor_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBRColor_PointLight.glsl");
		PBRshader->Bind();
		PBRshader->SetInt("material.diffuse", 0, ShaderType::PS);
		PBRshader->SetInt("material.specular", 1, ShaderType::PS);
		PBRshader->SetInt("material.normal", 2, ShaderType::PS);

		Sampler_Desc desc;
		Ref<Texture2D> brick = TextureLibrary::Load("Asset/textures/bricks2.jpg", desc);
		Ref<Texture2D> brick_normal = TextureLibrary::Load("Asset/textures/bricks2_normal.jpg",desc);

		Brick.SetTexture(brick, Material::MaterialType::Diffuse);
		Brick.SetTexture(brick_normal, Material::MaterialType::NormalMap);
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
		sphere->SetMaterial(Color);

		cube = CreateRef<Cube>(glm::vec3(1.0f, 0.0f, 0.0f), PBRshader);
		cube->SetMaterial(Color);
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
			PBRshader->SetFloat3(lightname +"specular", light[i].info.specular, ShaderType::PS);
			PBRshader->SetFloat3(lightname +"ambient", light[i].info.ambient, ShaderType::PS);

			PBRshader->SetFloat(lightname +"constant", light[i].info.constant, ShaderType::PS);
			PBRshader->SetFloat(lightname +"Linear", light[i].info.linear, ShaderType::PS);
			PBRshader->SetFloat(lightname +"quadratic", light[i].info.quadratic, ShaderType::PS);
		}
		

		PBRshader->SetFloat("material.ao", 1.0f, ShaderType::PS);
		PBRshader->SetFloat3("material.albedo", { 0.5f,0.0f,0.0f }, ShaderType::PS);

		sphere->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		for (int i = 0; i < 7; ++i)
		{
			PBRshader->SetFloat("material.metallic", (float)i / (float)7, ShaderType::PS);
			for (int j = 0; j < 7; ++j)
			{
				float roughness = glm::clamp((float)j / (float)7, 0.05f, 1.0f);
				PBRshader->SetFloat("material.roughness", roughness, ShaderType::PS);

				sphere->SetTranslation(glm::vec3(-1.05f+j*0.3f, 1.05f-i*0.3f, 0.0f));
				glm::mat4 transform = sphere->GetTransform();
				PBRshader->SetMat4("u_Transform", transform, ShaderType::VS);
				PBRshader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
				// material
				PBRshader->SetFloat("material.shininess", Color.shininess, ShaderType::PS);
				if (Color.IsThereTexture(Material::MaterialType::NormalMap))
					PBRshader->SetBool("material.normalMap", true, ShaderType::PS);
				else
					PBRshader->SetBool("material.normalMap", false, ShaderType::PS);
				PBRshader->UpdateBuffer();

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

