#include "../Header/RenderGraphTest.h"
#include "glm/gtc/type_ptr.hpp"

namespace QCat
{
	RenderGraphTest::RenderGraphTest()
	{

	}
	void RenderGraphTest::OnAttach()
	{
		sphere = CreateRef<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");

		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 0.0f,0.0f,-2.0f };

		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(1, 0.01f, 100.0f);
		Sampler_Desc imgSamp;
		imgSamp.addressU = WrapingMode::REPEAT;
		imgSamp.addressV = WrapingMode::REPEAT;
		imgSamp.MIN = Filtering::LINEAR;
		imgSamp.MAG = Filtering::LINEAR;
		imgSamp.MIP = Filtering::LINEAR;
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/normal.png", imgSamp, Material::TextureType::Normal);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[0].SetTexture("Asset/textures/PBR/rusted_iron/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[1].SetTexture("Asset/textures/PBR/gold/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[1].SetTexture("Asset/textures/PBR/gold/normal.png", imgSamp, Material::TextureType::Normal);
		materials[1].SetTexture("Asset/textures/PBR/gold/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[1].SetTexture("Asset/textures/PBR/gold/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[1].SetTexture("Asset/textures/PBR/gold/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[2].SetTexture("Asset/textures/PBR/grass/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[2].SetTexture("Asset/textures/PBR/grass/normal.png", imgSamp, Material::TextureType::Normal);
		materials[2].SetTexture("Asset/textures/PBR/grass/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[2].SetTexture("Asset/textures/PBR/grass/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[2].SetTexture("Asset/textures/PBR/grass/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[3].SetTexture("Asset/textures/PBR/plastic/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[3].SetTexture("Asset/textures/PBR/plastic/normal.png", imgSamp, Material::TextureType::Normal);
		materials[3].SetTexture("Asset/textures/PBR/plastic/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[3].SetTexture("Asset/textures/PBR/plastic/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[3].SetTexture("Asset/textures/PBR/plastic/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		materials[4].SetTexture("Asset/textures/PBR/wall/albedo.png", imgSamp, Material::TextureType::Diffuse);
		materials[4].SetTexture("Asset/textures/PBR/wall/normal.png", imgSamp, Material::TextureType::Normal);
		materials[4].SetTexture("Asset/textures/PBR/wall/metallic.png", imgSamp, Material::TextureType::Metallic);
		materials[4].SetTexture("Asset/textures/PBR/wall/roughness.png", imgSamp, Material::TextureType::Roughness);
		materials[4].SetTexture("Asset/textures/PBR/wall/ao.png", imgSamp, Material::TextureType::AmbientOcclusion);

		ball[0] = m_ActiveScene->CreateEntity("GoldenBall");
		ball[0].GetComponent<TransformComponent>().Translation = { -1.0f,0.0f,0.0f };
		ball[0].AddComponent<MeshComponent>("Sphere");
		ball[0].AddComponent<MaterialComponent>(materials[0]);

		ball[1] = m_ActiveScene->CreateEntity("rustyBall");
		ball[1].GetComponent<TransformComponent>().Translation = { -0.7f,0.0f,0.0f };
		ball[1].AddComponent<MeshComponent>("Sphere");
		ball[1].AddComponent<MaterialComponent>(materials[1]);

		ball[2] = m_ActiveScene->CreateEntity("grassBall");
		ball[2].GetComponent<TransformComponent>().Translation = { -0.4f,0.0f,0.0f };
		ball[2].AddComponent<MeshComponent>("Sphere");
		ball[2].AddComponent<MaterialComponent>(materials[2]);

		ball[3] = m_ActiveScene->CreateEntity("wallBall");
		ball[3].GetComponent<TransformComponent>().Translation = { -0.1f,0.0f,0.0f };
		ball[3].AddComponent<MeshComponent>("Sphere");
		ball[3].AddComponent<MaterialComponent>(materials[3]);

		ball[4] = m_ActiveScene->CreateEntity("plasticBall");
		ball[4].GetComponent<TransformComponent>().Translation = { 0.2f,0.0f,0.0f };
		ball[4].AddComponent<MeshComponent>("Sphere");
		ball[4].AddComponent<MaterialComponent>(materials[4]);

		Sampler_Desc desc;
		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		hdrImage = TextureLibrary::Load("Asset/textures/HdrImage/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", desc, 1, 1, RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? false : true);
		renderGraph.SetHdrImg(hdrImage);
		viewMatrix = CreateRef<glm::mat4>(glm::mat4(1.0f));
		renderGraph.SetView(viewMatrix);
		renderGraph.Initialize();
	}
	void RenderGraphTest::OnDetach()
	{
	}
	void RenderGraphTest::OnUpdate(Timestep ts)
	{
		CameraUpdate(ts);
		renderGraph.Execute(m_ActiveScene);
	}
	void RenderGraphTest::OnImGuiRender()
	{
	}
	void RenderGraphTest::OnEvent(Event& e)
	{
	}
	void RenderGraphTest::CameraUpdate(QCat::Timestep ts)
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


				pitch -= dy * 0.04f;
				glm::vec3 front;

				{
					front = { 0.0f,0.0f,1.0f };
					yaw -= dx * 0.04f;
				}

				//QCAT_INFO("dx : {0} , dy{1}", dx, dy);

				if (pitch > 89.0f)
					pitch = 89.f;
				if (pitch < -89.0f)
					pitch = -89.f;


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
		*viewMatrix = glm::lookAt(tc, tc + cameraFront, cameraUp);
	}
}