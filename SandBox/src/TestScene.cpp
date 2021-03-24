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
			BlinnPhongShader = ShaderLibrary::Load("Asset/shaders/glsl/Blinn-phong_MultipleLight.glsl");
			screenShader = ShaderLibrary::Load("Asset/shaders/glsl/Hdr/HdrShader.glsl");
			FlatShader = ShaderLibrary::Load("Asset/shaders/glsl/Blur/BrightFlatColor.glsl");
			GaussianBlur = ShaderLibrary::Load("Asset/shaders/glsl/Blur/GaussianBlur.glsl");
			BloomShader = ShaderLibrary::Load("Asset/shaders/glsl/Blur/HdrBloom.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			BlinnPhongShader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl", "Asset/shaders/hlsl/HDR/BlinnAndPhong_Multiplelight_Bright_PS.hlsl" );
			screenShader = ShaderLibrary::Load("HDRShader", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/HDR/HdrShader_PS.hlsl");
			FlatShader = ShaderLibrary::Load("FlatShader", "Asset/shaders/hlsl/flatcolor_VS.hlsl", "Asset/shaders/hlsl/Blur/LightBox_PS.hlsl");
			GaussianBlur = ShaderLibrary::Load("Gaussian", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/Blur/GaussianBlur_PS.hlsl");
			BloomShader = ShaderLibrary::Load("BloomShader", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/Blur/HdrShaderBloom_PS.hlsl");
		}

		BlinnPhongShader->Bind();
		BlinnPhongShader->SetInt("material.diffuse", 0, ShaderType::PS);
		BlinnPhongShader->SetInt("material.specular", 1, ShaderType::PS);
		BlinnPhongShader->SetInt("material.normal", 2, ShaderType::PS);

		screenShader->Bind();
		screenShader->SetInt("hdrBuffer", 0, ShaderType::PS);

		GaussianBlur->Bind();
		GaussianBlur->SetInt("image", 0, ShaderType::PS);

		BloomShader->Bind();
		BloomShader->SetInt("hdrBuffer", 0, ShaderType::PS);
		BloomShader->SetInt("bloomBlur", 1, ShaderType::PS);

		//floor Texture
		floorTexture = TextureLibrary::Load("Asset/textures/floor.png",false,true);

		floor.SetTexture(floorTexture, Material::MaterialType::Diffuse);
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f), BlinnPhongShader);
		LightPosition = glm::vec3(0.0f, 0.0f, -2.0f);

		LightInfo info;
		info.lightPosition = glm::vec3(0.0f, 0.0f, 4.5f);
		info.diffuse = glm::vec3(200.0f, 200.0f, 200.0f);
		info.constant = 0.1f;
		info.linear = 2.0f;
		info.quadratic = 12.0f;
		
		light[0].SetLightinfo(info);

		light[1].SetPosition(glm::vec3(-0.4f, -0.4f, -3.0f));
		light[2].SetPosition(glm::vec3(0.4f, -0.4f, -2.0f));
		light[3].SetPosition(glm::vec3(-0.4f, -0.4f, -0.5f));

		light[1].SetDiffuse(glm::vec3(0.4f, 0.0f, 0.0f));
		light[2].SetDiffuse(glm::vec3(0.0f, 0.4f, 0.0f));
		light[3].SetDiffuse(glm::vec3(0.0f, 0.0f, 0.4f));

		light[1].SetSpecular(glm::vec3(0.4f, 0.0f, 0.0f));
		light[2].SetSpecular(glm::vec3(0.0f, 0.4f, 0.0f));
		light[3].SetSpecular(glm::vec3(0.0f, 0.0f, 0.4f));

		for (int i = 1; i < 4; ++i)
		{
			light[i].info.constant = 0.1f;
			light[i].info.linear = 2.0f;
			light[i].info.quadratic = 4.0f;
		}

		FrameBufferSpecification spec;
		spec.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureDataFormat::RGBA16_Float},
							 {FramebufferUsage::Color,TextureType::Texture2D,TextureDataFormat::RGBA16_Float},
						     {FramebufferUsage::Depth_Stencil ,TextureType::Texture2D,TextureDataFormat::DEPTH24STENCIL8} };
		spec.Width = 1600;
		spec.Height = 900;
		offrendering = FrameBuffer::Create(spec);

		FrameBufferSpecification spec2;
		spec2.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureDataFormat::RGBA16_Float} };
		spec2.Width = 1600;
		spec2.Height = 900;

		pingpongBuffer[0] = FrameBuffer::Create(spec2);
		pingpongBuffer[1] = FrameBuffer::Create(spec2);

		m_quad = VertexArray::Create();

		static float bias = 0.0f;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			bias = 1.0f;
		}
		float quadVertices[] =
		{
			-1.0f, 1.0f,  0.0f,0.0f + bias,//0
			-1.0f,-1.0f,  0.0f,1.0f - bias,//1
			 1.0f,-1.0f,  1.0f,1.0f - bias,//2
			 1.0f, 1.0f,  1.0f,0.0f + bias // 3
		};

		Ref<VertexBuffer> quadBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));

		quadBuffer->SetLayout(BufferLayout::Create({
			{ShaderDataType::Float2,"a_Position"},
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

		sphere = CreateRef<Sphere>(glm::vec3(0.0f,0.0f,0.0f), FlatShader, 0.1f);
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
			
			QCat::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			QCat::RenderCommand::Clear();
		}
		{
			RenderCommand::SetCullMode(CullMode::None);
			RenderCommand::SetDepthTest(true);

			QCAT_PROFILE_SCOPE("Renderer Draw");
			const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;

			offrendering->Bind();
			offrendering->Clear({ 0.0f, 0.0f, 0.0f, 1.0f });

			BlinnPhongShader->Bind();
			BlinnPhongShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			BlinnPhongShader->SetFloat3("viewPosition", tc, ShaderType::VS);
			for (int i = 0; i < 4; ++i)
			{
				std::string lightname = "pointLight[" + std::to_string(i) + "].";
				// Point Light 
				BlinnPhongShader->SetFloat3(lightname +"position",light[i].Getinfo().lightPosition, ShaderType::PS);
				BlinnPhongShader->SetFloat3(lightname +"ambient", light[i].Getinfo().ambient, ShaderType::PS);
				BlinnPhongShader->SetFloat3(lightname +"diffuse", light[i].Getinfo().diffuse, ShaderType::PS);
				BlinnPhongShader->SetFloat3(lightname +"specular", light[i].Getinfo().specular, ShaderType::PS);

				BlinnPhongShader->SetFloat(lightname + "constant", light[i].Getinfo().constant, ShaderType::PS);
				BlinnPhongShader->SetFloat(lightname + "Linear", light[i].Getinfo().linear, ShaderType::PS);
				BlinnPhongShader->SetFloat(lightname + "quadratic", light[i].Getinfo().quadratic, ShaderType::PS);
			}
			/*BlinnPhongShader->SetBool("flip", false, ShaderType::PS);
			face->SetTranslation({ -1.0f,0.0f,0.0f });
			face->SetMaterial(brick2);
			face->MaterialBind();
			face->Draw(BlinnPhongShader);*/

			BlinnPhongShader->SetBool("flip",true,ShaderType::PS);
			cube->SetTranslation({ 0.0f,0.0f,0.0f });
			cube->SetScale({ 1.0f,1.0f,10.0f });
			cube->SetMaterial(floor);
			cube->Draw(BlinnPhongShader);

			BlinnPhongShader->UnBind();

			RenderCommand::SetCullMode(CullMode::Back);
			FlatShader->Bind();
			FlatShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			FlatShader->SetFloat4("u_color", glm::vec4(5.0f, 5.0f, 5.0f, 1.0f), ShaderType::PS);
			sphere->SetTranslation(light[0].info.lightPosition);
			sphere->SetScale({ 0.5f,0.5f,0.5f });
			sphere->Draw(FlatShader);

			FlatShader->SetFloat4("u_color", glm::vec4(10.0f, 0.0f, 0.0f, 1.0f), ShaderType::PS);
			sphere->SetTranslation(light[1].info.lightPosition);
			sphere->Draw(FlatShader);

			FlatShader->SetFloat4("u_color", glm::vec4(0.0f, 5.0f, 0.0f, 1.0f), ShaderType::PS);
			sphere->SetTranslation(light[2].info.lightPosition);
			sphere->Draw(FlatShader);

			FlatShader->SetFloat4("u_color", glm::vec4(0.0f, 0.0f, 15.0f, 1.0f), ShaderType::PS);
			sphere->SetTranslation(light[3].info.lightPosition);
			sphere->Draw(FlatShader);

			FlatShader->UnBind();
			offrendering->UnBind();

			GaussianBlur->Bind();
			for (int i = 0; i < 10; ++i)
			{
				int index = 0;
				if (horizontal)
				{
					index = 0;
					horizontal = false;
				}
				else
				{
					index = 1;
					horizontal = true;
				}
				pingpongBuffer[index]->Bind();
				GaussianBlur->SetBool("horizontal", horizontal,ShaderType::PS);
				if (first_iteration)
				{
					offrendering->BindColorTexture(0, 1);
					first_iteration = false;
				}
				else
				{
					if (index == 0)
						pingpongBuffer[1]->BindColorTexture(0, 0);
					else
						pingpongBuffer[0]->BindColorTexture(0, 0);
				}
				m_quad->Bind();
				GaussianBlur->UpdateBuffer();
				RenderCommand::DrawIndexed(m_quad);
				pingpongBuffer[index]->UnBind();
			}	
			first_iteration = true;
			GaussianBlur->UnBind();

			RenderCommand::Clear();
			RenderCommand::SetDefaultFrameBuffer();

			RenderCommand::SetDepthTest(false);
			BloomShader->Bind();
			BloomShader->SetFloat("exposure", exposure, ShaderType::PS);
			offrendering->BindColorTexture(0, 0);
			pingpongBuffer[0]->BindColorTexture(1, 0);
			//floorTexture->Bind(0);
			m_quad->Bind();
			BloomShader->UpdateBuffer();
			RenderCommand::DrawIndexed(m_quad);
			BloomShader->UnBind();
			offrendering->UnBindTexture();
			
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
		ImGui::DragFloat("Exposure", &exposure, 0.1f);
		
		ImGui::End();

		for (int i = 0; i < 4; ++i)
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
		if (width > 0.0f && height > 0.0f && (spec.Width != width || spec.Height != height))
		{
			offrendering->Resize(width, height);
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
