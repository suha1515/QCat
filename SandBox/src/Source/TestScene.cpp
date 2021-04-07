#include "../Header/TestScene.h"

#include <Imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/detail/type_half.hpp>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

#include <chrono>
#include <QCat/InputDevice/Mouse/Mouse.h>

#include <random>

float GetRandomNumber(float min, float max)
{
	std::random_device rn;
	std::mt19937_64 rnd(rn());

	std::uniform_real_distribution<float> range(min, max);

	return range(rnd);
}
namespace QCat
{
	TestScene::TestScene()
		:Layer("TestScene")
	{
		
	}
	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}
	void TestScene::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");
		
		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 2.3f,0.1f,-3.0f };
		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.01f, 100.0f);

		//model load
		backpack = Model::Create("Asset/model/backpack/backpack.obj");

		//Shader
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			BlinnPhongShader = ShaderLibrary::Load("Asset/shaders/glsl/Blinn-phong_MultipleLight.glsl");
			screenShader = ShaderLibrary::Load("Asset/Shaders/glsl/SingleQuad.glsl");
			//FlatShader = ShaderLibrary::Load("Asset/shaders/glsl/Blur/BrightFlatColor.glsl");
			FlatShader = ShaderLibrary::Load("Asset/shaders/glsl/FlatColor.glsl");
			GeometryPass = ShaderLibrary::Load("Asset/shaders/glsl/DeferredShading/GeometryPass_difspecnorm.glsl");
			DeferredLighting = ShaderLibrary::Load("Asset/shaders/glsl/DeferredShading/DeferredShade.glsl");
			SSaoWrite = ShaderLibrary::Load("Asset/shaders/glsl/SSAO/SSAO.glsl");
			SSaoBlur = ShaderLibrary::Load("Asset/shaders/glsl/SSAO/SSAO_Blur.glsl");
			SSaoLight = ShaderLibrary::Load("Asset/shaders/glsl/SSAO/SSAO_Lighting.glsl");
			SSaogeo = ShaderLibrary::Load("Asset/shaders/glsl/SSAO/SSAO_geometry.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			BlinnPhongShader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl", "Asset/shaders/hlsl/HDR/BlinnAndPhong_Multiplelight_Bright_PS.hlsl" );
			screenShader = ShaderLibrary::Load("ScreenShader", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SingleQuad_PS.hlsl");
			//FlatShader = ShaderLibrary::Load("FlatShader", "Asset/shaders/hlsl/flatcolor_VS.hlsl", "Asset/shaders/hlsl/Blur/LightBox_PS.hlsl");
			FlatShader = ShaderLibrary::Load("FlatShader", "Asset/shaders/hlsl/flatcolor_VS.hlsl", "Asset/shaders/hlsl/flatcolor_PS.hlsl");
			DeferredLighting = ShaderLibrary::Load("DeferredLight", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/DeferredShading/DeferredShade.hlsl");
			GeometryPass = ShaderLibrary::Load("GeoMeteryPass", "Asset/shaders/hlsl/PosNormTexTanBi_FragTexTbnNorm_VS.hlsl", "Asset/shaders/hlsl/DeferredShading/GeometryPass.hlsl");
			//SSaoWrite = ShaderLibrary::Load("SsaoWrite","Asset/shaders/hlsl/SingleQuad_VS.hlsl","SSAO_PS.cso");
			SSaoWrite = ShaderLibrary::Load("SsaoWrite", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SSAO/SSAO_PS.hlsl");
			SSaoBlur = ShaderLibrary::Load("SsaoBlur", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SSAO/SSAO_Blur_PS.hlsl");
			SSaoLight = ShaderLibrary::Load("SsaoLight", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SSAO/SSAO_Lighting_PS.hlsl");
			SSaogeo = ShaderLibrary::Load("SsaoGeo", "Asset/shaders/hlsl/SSAO/SSAO_geometry_VS.hlsl", "Asset/shaders/hlsl/SSAO/SSAO_geometry_PS.hlsl");
			//SSaogeo = ShaderLibrary::Load("SsaoGeo", "SSAO_geometry_VS.cso", "SSAO_geometry_PS.cso");

		}

		BlinnPhongShader->Bind();
		BlinnPhongShader->SetInt("material.diffuse", 0, ShaderType::PS);
		BlinnPhongShader->SetInt("material.specular", 1, ShaderType::PS);
		BlinnPhongShader->SetInt("material.normal", 2, ShaderType::PS);

		screenShader->Bind();
		screenShader->SetInt("screenTexture", 0, ShaderType::PS);

		SSaogeo->Bind();
		SSaogeo->SetInt("texture_diffuse", 0, ShaderType::PS);
		SSaogeo->SetInt("texture_specular", 1, ShaderType::PS);
		SSaogeo->SetInt("texture_normal", 2, ShaderType::PS);

		DeferredLighting->Bind();
		DeferredLighting->SetInt("gPosition", 0, ShaderType::PS);
		DeferredLighting->SetInt("gNormal", 1, ShaderType::PS);
		DeferredLighting->SetInt("aAlbedoSpec", 2, ShaderType::PS);

		SSaoWrite->Bind();
		SSaoWrite->SetInt("gPosition", 0, ShaderType::PS);
		SSaoWrite->SetInt("gNormal", 1, ShaderType::PS);
		SSaoWrite->SetInt("texNoise", 2, ShaderType::PS);

		SSaoBlur->Bind();
		SSaoBlur->SetInt("ssaoInput", 0, ShaderType::PS);

		SSaoLight->Bind();
		SSaoLight->SetInt("gPosition", 0, ShaderType::PS);
		SSaoLight->SetInt("gNormal", 1, ShaderType::PS);
		SSaoLight->SetInt("gAlbedo", 2, ShaderType::PS);
		SSaoLight->SetInt("ssao", 3, ShaderType::PS);

		//floor Texture
		Sampler_Desc desc;
		floorTexture = TextureLibrary::Load("Asset/textures/floor.png",desc);
		Ref<Texture2D> brickTex = TextureLibrary::Load("Asset/textures/bricks2.jpg", desc);
		Ref<Texture2D> brickNormTex = TextureLibrary::Load("Asset/textures/bricks2_normal.jpg");

		floor.SetTexture(floorTexture, Material::TextureType::Diffuse);
		floor.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		brick.SetTexture(brickTex, Material::TextureType::Diffuse);
		noTex.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		brick.SetTexture(brickNormTex, Material::TextureType::Normal);
		brick.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);

		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f), BlinnPhongShader);
		
		LightPosition = glm::vec3(-1.4f, -1.1f, 1.9f);
			
		light[0].info.lightPosition = LightPosition;
		light[0].info.linear = 0.09f;
		light[0].info.constant = 1.0f;
		light[0].info.quadratic = 0.032f;
		light[0].info.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		light[0].info.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		light[0].info.specular = glm::vec3(1.0f, 1.0f, 1.0f);

		float lightMax = std::fmaxf(std::fmaxf(light[0].info.diffuse.r, light[0].info.diffuse.g), light[0].info.diffuse.b);
		light[0].info.radius = (-Linear + std::sqrtf(Linear * Linear - 4 * quadratic * (constant - (256.0f / 5.0f) * lightMax)))
			/ (2 * quadratic);

		FrameBufferSpecification spec;
		spec.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA16_Float},
							 {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA16_Float},
							 {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8},
							 {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA16_Float},
							 {FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8} };
		spec.Width = 1600;
		spec.Height = 900;
		offrendering = FrameBuffer::Create(spec);
		Sampler_Desc nearClamp;
		nearClamp.MIN = Filtering::POINT;
		nearClamp.MAG = Filtering::POINT;
		nearClamp.addressU = WrapingMode::CLAMP;
		nearClamp.addressV = WrapingMode::CLAMP;
		offrendering->GetColorTexture(0)->sampler->SetSamplerDesc(nearClamp);

		spec.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RED32_FLOAT}};
		spec.Width = 1600;
		spec.Height = 900;
		screenSize.x = 1600;
		screenSize.y = 900;
		Sampler_Desc nearPoint;
		nearPoint.MIN = Filtering::POINT;
		nearPoint.MAG = Filtering::POINT;
		ssao = FrameBuffer::Create(spec);
		ssaoBlur = FrameBuffer::Create(spec);
		ssao->GetColorTexture(0)->sampler->SetSamplerDesc(nearPoint);
		ssaoBlur->GetColorTexture(0)->sampler->SetSamplerDesc(nearPoint);
		
	
		m_quad = VertexArray::Create();

		static float bias = 0.0f;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			bias = 1.0f;
		}
		float quadVertices[] =
		{
			-1.0f, 1.0f,0.0f,  0.0f,0.0f + bias,//0
			-1.0f,-1.0f,0.0f,  0.0f,1.0f - bias,//1
			 1.0f,-1.0f,0.0f,  1.0f,1.0f - bias,//2
			 1.0f, 1.0f,0.0f,  1.0f,0.0f + bias // 3
		};

		Ref<VertexBuffer> quadBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));

		quadBuffer->SetLayout(BufferLayout::Create({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoords"} }, screenShader
			));

		unsigned int indices[] =
		{
			1,0,3,1,3,2
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6);

		m_quad->AddVertexBuffer(quadBuffer);
		m_quad->SetIndexBuffer(indexBuffer);
		m_quad->UnBind();

		sphere = CreateRef<Sphere>(glm::vec3(0.0f,0.0f,0.0f), 0.1f);

		face = CreateRef<Face>(glm::vec3(0.0f, 0.0f, 0.0f), BlinnPhongShader, floor);

		linearClamp = SamplerState::Create(desc);

		// generate Sample kernel
		std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f);
		std::default_random_engine generator;
		for (int i = 0; i < 128; ++i)
		{
			glm::vec3 sample(randomFloat(generator) * 2.0f - 1.0f, 
							randomFloat(generator) * 2.0f - 1.0f, 
							randomFloat(generator));
			sample = glm::normalize(sample);
			sample *= randomFloat(generator);
			float scale = float(i) / 64.0f;

			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaokernel.push_back(sample);
		}

		//SSAO noise Texture
		for (int i = 0; i < 16; ++i)
		{
			glm::vec4 noise({ randomFloat(generator) * 2.0f - 1.0f,randomFloat(generator) * 2.0f - 1.0f
				,0.0f,0.0f});
			ssaoNoise.push_back(noise);
		}
		f16Vec4 b[16];
		for(int i=0;i<16;++i)
		{
			b[i] = ssaoNoise[i];
		}
			 
		TextureFormat format = TextureFormat::RGBA16_Float;
		Sampler_Desc desc2;
		desc2.MIN = Filtering::POINT;
		desc2.MAG = Filtering::POINT;
		desc2.MIP = Filtering::POINT;
		desc2.addressU = WrapingMode::REPEAT;
		desc2.addressV = WrapingMode::REPEAT;
		desc2.addressW = WrapingMode::REPEAT;
		noiseTexture = Texture2D::Create(format, desc2, 4, 4,1,1, &b[0]);

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
	
		QCAT_PROFILE_SCOPE("Renderer Prep");
			
		QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		QCat::RenderCommand::Clear();
		{
			RenderCommand::SetDepthTest(true);
			RenderCommand::SetCullMode(CullMode::None);

			QCAT_PROFILE_SCOPE("Renderer Draw");
			const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;

			offrendering->Bind();
			offrendering->Clear();
			SSaogeo->Bind();
			SSaogeo->SetMat4("view", viewMatrix, ShaderType::VS);
			SSaogeo->SetMat4("invView", glm::inverse(viewMatrix), ShaderType::VS);
			SSaogeo->SetMat4("proj", camProj , ShaderType::VS);

			SSaogeo->SetBool("flip", true, ShaderType::VS);

			cube->SetTranslation({ 0.0f,0.0f,0.0f });
			cube->SetScale({ 20.0f,5.0f,20.0f });
			cube->SetMaterial(floor);
			cube->Draw(SSaogeo);

			SSaogeo->SetBool("flip", false, ShaderType::VS);

			noTex.diffuse = (glm::vec3(1.0f, 1.0f, 0.0f));
			cube->SetTranslation({ -2.5f,-2.f,-1.0f });
			cube->SetScale({1.0f,1.0f,1.0f});
			cube->SetMaterial(brick);
			cube->Draw(SSaogeo);

			noTex.diffuse = (glm::vec3(1.0f, 0.0f, 1.0f));
			cube->SetTranslation({ -3.5f,-2.f, 1.0f});
			cube->SetScale({ 1.0f,1.0f,1.0f });
			cube->SetMaterial(brick);
			cube->Draw(SSaogeo);

			backpack->SetTranslation(backpackPos);
			backpack->SetRotation(backpackRot);
			backpack->SetScale({ 0.5f,0.5f,0.5f });
			backpack->Draw();

			SSaogeo->UnBind();
			offrendering->UnBind();

			// use G-Buffer to render SSAO Texture
			ssao->Bind();
			ssao->Clear();
			SSaoWrite->Bind();
			offrendering->BindColorTexture(0, 0);
			offrendering->BindColorTexture(1, 1);
			noiseTexture->Bind(2);
			for (int i = 0; i < 128; ++i)
				SSaoWrite->SetFloat3("samples[" + std::to_string(i) + "]", ssaokernel[i],ShaderType::PS);
			SSaoWrite->SetMat4("Projection", camProj, ShaderType::PS);
			SSaoWrite->SetFloat2("screenSize",screenSize, ShaderType::PS);
			SSaoWrite->SetFloat("radius", radius, ShaderType::PS);
			SSaoWrite->SetFloat("bias", bias, ShaderType::PS);

			m_quad->Bind();
			RenderCommand::DrawIndexed(m_quad);
			SSaoWrite->UnBind();
			ssao->UnBind();

			ssaoBlur->Bind();
			SSaoBlur->Bind();
			ssao->BindColorTexture(0, 0);
			m_quad->Bind();
			RenderCommand::DrawIndexed(m_quad);
			SSaoBlur->UnBind();
			ssaoBlur->UnBind();

			RenderCommand::SetDefaultFrameBuffer();

			SSaoLight->Bind();
			SSaoLight->SetFloat3("viewPosition", tc, ShaderType::PS);

			for (int i = 0; i < 1; ++i)
			{
				std::string lightname = "pointLight[" + std::to_string(i) + "].";
				// Point Light 
				SSaoLight->SetFloat3(lightname + "position",  glm::vec3(viewMatrix * glm::vec4(light[i].Getinfo().lightPosition,1.0f)), ShaderType::PS);
				SSaoLight->SetFloat3(lightname + "ambient" , light[i].Getinfo().ambient, ShaderType::PS);
				SSaoLight->SetFloat3(lightname + "diffuse" , light[i].Getinfo().diffuse, ShaderType::PS);
				SSaoLight->SetFloat3(lightname + "specular", light[i].Getinfo().specular, ShaderType::PS);

				SSaoLight->SetFloat(lightname + "constant", light[i].info.constant , ShaderType::PS);
				SSaoLight->SetFloat(lightname + "Linear"   , light[i].info.linear, ShaderType::PS);
				SSaoLight->SetFloat(lightname + "quadratic", light[i].info.quadratic, ShaderType::PS);
				SSaoLight->SetFloat(lightname + "radius", light[i].info.radius, ShaderType::PS);
			}
			offrendering->BindColorTexture(0,0);
			offrendering->BindColorTexture(1,1);
			offrendering->BindColorTexture(2,2);
			ssaoBlur->BindColorTexture(3, 0);

			m_quad->Bind();
			RenderCommand::DrawIndexed(m_quad);
			DeferredLighting->UnBind();


			FrameBufferSpecification spec = offrendering->GetSpecification();
			offrendering->CopyFrameBuffer(0, 0, spec.Width, spec.Height, 0, 0, spec.Width, spec.Height, BufferBit::Depth, nullptr);

			FlatShader->Bind();
			FlatShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			sphere->SetScale({ 0.5f,0.5f,0.5f });
			for (int i = 0; i < 1; ++i)
			{
				FlatShader->SetFloat4("u_color", glm::vec4(light[i].info.diffuse, 1.0f), ShaderType::PS);
				sphere->SetTranslation(light[i].info.lightPosition);
				sphere->Draw(FlatShader);
			}
			FlatShader->UnBind();

			screenShader->Bind();
			ssaoBlur->BindColorTexture(0, 0);
			m_quad->Bind();
			RenderCommand::DrawIndexed(m_quad);
			screenShader->UnBind();
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
		ImGui::DragFloat3("backpackPos", glm::value_ptr(backpackPos), 0.1f);
		ImGui::DragFloat3("backpackRot", glm::value_ptr(backpackRot), 0.1f);
		ImGui::DragFloat("radius", &radius, 0.01f);
		ImGui::DragFloat("bias", &bias, 0.01f);
		
		ImGui::End();

		for (int i = 0; i < 1; ++i)
		{
			std::string lightname = "pointLight[" + std::to_string(i) + "]";
			light[i].ImGuiRender(lightname.c_str());
		}
	}

	void TestScene::OnEvent(QCat::Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

	}

	bool TestScene::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();
		FrameBufferSpecification spec = offrendering->GetSpecification();
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		screenSize = glm::vec2(width, height);
		if (width > 0.0f && height > 0.0f && (spec.Width != width || spec.Height != height))
		{
			offrendering->Resize(width, height);
			ssao->Resize(width, height);
			ssaoBlur->Resize(width, height);
		}
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
