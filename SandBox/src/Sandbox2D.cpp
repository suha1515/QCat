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
		camera.Camera.SetPerspective(glm::radians(30.0f), 0.01f, 100.0f);


		//sphere = CreateRef<Sphere>(glm::vec3(-3.0f, 0.0f, 3.0f));

		//light
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			m_LightShader = ShaderLibrary::Load("Asset/shaders/glsl/Blinn-phong.glsl");
			m_FlatShader = ShaderLibrary::Load("Asset/shaders/glsl/FlatShader.glsl");
			m_ScreenShader = ShaderLibrary::Load("Asset/shaders/glsl/SingleQuad.glsl");
			m_SkyBoxShader = ShaderLibrary::Load("Asset/shaders/glsl/SkyBox.glsl");
			m_ReflectShader = ShaderLibrary::Load("Asset/shaders/glsl/Reflective.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			m_LightShader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/BlinnAndPhong_VS.hlsl", "Asset/shaders/hlsl/BlinnAndPhong_PS.hlsl");
			m_FlatShader = ShaderLibrary::Load("FlatShader", "Asset/shaders/hlsl/PosNormTcFrag_TransInvTrans.hlsl", "Asset/shaders/hlsl/flatcolor_PS.hlsl");
			m_ScreenShader = ShaderLibrary::Load("QuadShader", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SingleQuad_PS.hlsl");
			m_SkyBoxShader = ShaderLibrary::Load("SkyBoxShader","Asset/shaders/hlsl/SkyBox_VS.hlsl","Asset/shaders/hlsl/SkyBox_PS.hlsl");
			m_ReflectShader = ShaderLibrary::Load("ReflectShader", "Asset/shaders/hlsl/Reflect_VS.hlsl", "Asset/shaders/hlsl/Reflect_PS.hlsl");

		}

		//material.SetTexutre("Asset/textures/matrix.jpg", Material::MaterialType::Emission);

		woodFloor = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		grass     = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		window    = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		brick	  = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		Box	      = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		BasicMaterial = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		ReflectMaterial = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		// wood Texture
		Ref<Texture2D> woodTexture = TextureLibrary::Load("Asset/textures/floor.png");
		// grass Texture
		Ref<Texture2D> grassTexture = TextureLibrary::Load("Asset/textures/grass.png"); 
		// window Texture
		Ref<Texture2D> windowTexture = TextureLibrary::Load("Asset/textures/blending_transparent_window.png");
		// Brickwall Texture
		Ref<Texture2D> brickTexture = TextureLibrary::Load("Asset/textures/brickwall.jpg");
		// Brickwall normalmap
		Ref<Texture2D> brickNormalMap = TextureLibrary::Load("Asset/textures/brickwall_normal.jpg");
		// Box diffuse
		Ref<Texture2D> boxDiffuse = TextureLibrary::Load("Asset/textures/container2.png");
		// Box specular
		Ref<Texture2D> boxSpecular = TextureLibrary::Load("Asset/textures/container2_specular.png");
		// Sky Box
		std::vector<std::string> imagePath =
		{
			"Asset/textures/skybox/right.jpg",
			"Asset/textures/skybox/left.jpg",
			"Asset/textures/skybox/top.jpg",
			"Asset/textures/skybox/bottom.jpg",
			"Asset/textures/skybox/front.jpg",
			"Asset/textures/skybox/back.jpg",
		};
		m_cubeTexture = TextureCube::Create(imagePath);
		// quad Texture
		woodFloor.SetTexture(woodTexture, Material::MaterialType::Diffuse);
		grass.SetTexture(grassTexture, Material::MaterialType::Diffuse);
		window.SetTexture(windowTexture, Material::MaterialType::Diffuse);
		brick.SetTexture(brickTexture, Material::MaterialType::Diffuse);
		brick.SetTexture(brickNormalMap, Material::MaterialType::NormalMap);
		Box.SetTexture(boxDiffuse, Material::MaterialType::Diffuse);
		Box.SetTexture(boxSpecular, Material::MaterialType::Specular);
		ReflectMaterial.SetTexture(m_cubeTexture, Material::MaterialType::Diffuse);
		// model Load
		//bagPack = Model::Create("Asset/model/backpack/backpack.obj");
		//bagPack->SetTranslation({ 0.0f,0.0f,0.0f });
		//temMat = bagPack->GetMaterial();

		m_LightShader->Bind();
		m_LightShader->SetInt("material.diffuse", 0, ShaderType::PS);
		m_LightShader->SetInt("material.specular", 1, ShaderType::PS);
		m_LightShader->SetInt("material.normal", 2, ShaderType::PS);

		m_ScreenShader->Bind();
		m_ScreenShader->SetInt("screenTexture", 0, ShaderType::PS);

		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("skybox", 0, ShaderType::PS);

		m_ReflectShader->Bind();
		m_ReflectShader->SetInt("skybox", 0, ShaderType::PS);

		face = CreateRef<Face>(glm::vec3(0.0f, -3.0f, 0.0f),m_LightShader,woodFloor,1);
		face->SetScale({ 5.0f, 5.0f, 5.0f });
		sphere = CreateRef<Sphere>(glm::vec3(-0.1f, -2.6f, -1.0f), m_FlatShader, 0.1f);
		LightPosition = sphere->GetTranslation();
		cube = CreateRef<Cube>(glm::vec3(-1.6f, -2.6f, -0.6f), m_LightShader);
		cube->SetScale({ 0.5f,0.5f,0.5f });

		
		cubeMap = CreateRef<CubeMap>(imagePath,m_SkyBoxShader);

		floor = glm::vec3(0.0f, -3.0f, 0.0f);
		floorRot = glm::vec3(0.0f);
		brickwall = glm::vec3(0.0f, -1.6f, 2.42f);

		grass1 = glm::vec3(0.0f, -2.75f, 0.0f);
		grass2 = glm::vec3(0.8f, -2.75f, 0.4f);
		grass3 = glm::vec3(1.0f, -2.75f, -0.5f);

		cube1 = glm::vec3(-1.6f, -2.6f, -0.6f);
		cube2 = glm::vec3(-0.8f, -2.6f, 0.0f);
		
		window1 = glm::vec3(0.9f, -2.75f, -0.7f);
		window2 = glm::vec3(0.3f, -2.75f, 0.3f);
		window3 = glm::vec3(0.5f, -2.75f, -0.1f);

		backpackPos = glm::vec3(2.0f, -2.0f, 0.0f);
		ReflectObjPos = glm::vec3(0.5f, -2.0f, -0.5f);

		FrameBufferSpecification spec;
		spec.Attachments = { FramebufferTextureFormat::RGBA8,FramebufferTextureFormat::Depth };
		spec.Width = 1600;
		spec.Height = 900;
		framebuffer = FrameBuffer::Create(spec);

		m_quad = VertexArray::Create();

		static float bias = 0.0f;
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			bias = 1.0f;
		}
		float quadVertices[] =
		{
			-1.0f, 1.0f,  0.0f,0.0f+bias,//0
			-1.0f,-1.0f,  0.0f,1.0f-bias,//1
			 1.0f,-1.0f,  1.0f,1.0f-bias,//2
			 1.0f, 1.0f,  1.0f,0.0f+bias // 3
		};

		Ref<VertexBuffer> quadBuffer = VertexBuffer::Create(quadVertices,sizeof(quadVertices));

		quadBuffer->SetLayout(BufferLayout::Create({ 
			{ShaderDataType::Float2,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoords"}},m_ScreenShader
		));

		unsigned int indices[] =
		{
			1,0,3,1,3,2
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6);



		m_quad->AddVertexBuffer(quadBuffer);
		m_quad->SetIndexBuffer(indexBuffer);
		m_quad->UnBind();

		//FrameBuffer

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
			// new framebuffer
			framebuffer->Bind();
			QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			QCat::RenderCommand::Clear();
		}
		{
			QCAT_PROFILE_SCOPE("Renderer Draw");
			RenderCommand::SetDepthTest(true);
			RenderCommand::SetBlend(true);
			RenderCommand::SetBlendFunc(BlendFunc::BLEND_SRC_ALPHA, BlendFunc::BLEND_INV_SRC_ALPHA, BlendFunc::BLEND_ONE, BlendFunc::BLEND_ZERO);
			RenderCommand::SetBlendOp(BlendOp::BLEND_ADD, BlendOp::BLEND_ADD);

			const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;

			m_LightShader->Bind();
			m_LightShader->SetMat4("u_ViewProjection", camProj* viewMatrix,ShaderType::VS);
			m_LightShader->SetFloat3("viewPosition", tc, ShaderType::VS);
			m_LightShader->SetBool("blinn", blinn, ShaderType::PS);
			m_LightShader->SetBool("gamma", gamma, ShaderType::PS);

			// Point Light 1
			m_LightShader->SetFloat3("pointLight.position", LightPosition, ShaderType::PS);
			m_LightShader->SetFloat3("lightPosition", LightPosition, ShaderType::VS);
			m_LightShader->SetFloat3("pointLight.ambient", glm::vec3(1.0f, 0.6f, 0.0f)*0.1f, ShaderType::PS);
			m_LightShader->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
			m_LightShader->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);

			m_LightShader->SetFloat("pointLight.constant", constant, ShaderType::PS);
			m_LightShader->SetFloat("pointLight.Linear", Linear, ShaderType::PS);
			m_LightShader->SetFloat("pointLight.quadratic", quadratic, ShaderType::PS);

			face->SetMaterial(woodFloor);
			face->SetTranslation(floor);
			face->SetScale({ 5.0f,5.0f,1.0f });
			face->SetRotation({1.6f,0.0f,0.0f });
			face->Draw(m_LightShader);
			face->SetMaterial(brick);
			face->SetTranslation(brickwall);
			face->SetRotation(floorRot);
			face->SetScale({ 5.0f,3.0f,1.0f });
			face->Draw(m_LightShader);

			face->SetMaterial(grass);
			face->SetScale({ 0.5f,0.5f,0.5f });
			face->SetRotation({0.0f,0.0f,0.0f });
			face->SetTranslation(grass1);
			face->Draw(m_LightShader);
			face->SetTranslation(grass2);
			face->Draw(m_LightShader);
			face->SetTranslation(grass3);
			face->Draw(m_LightShader);

			face->SetMaterial(window);
			face->SetTranslation(window2);
			face->Draw(m_LightShader);
			face->SetTranslation(window3);
			face->Draw(m_LightShader);
			face->SetTranslation(window1);
			face->Draw(m_LightShader);
			//bagPack->SetTranslation(backpackPos);
			//bagPack->SetScale({ 0.5f, 0.5f, 0.5f });
			//bagPack->Draw(m_LightShader);

			RenderCommand::SetStencilTest(true);
			RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::ALWAYS, 1);
			RenderCommand::SetFrontStencilOp(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::REPLACE);
			RenderCommand::SetStencilWriteMask(0xFF);
			cube->SetMaterial(Box);
			cube->SetScale({ 0.5f, 0.5f, 0.5f });
			cube->SetTranslation(cube1);
			cube->Draw(m_LightShader);
			cube->SetMaterial(brick);
			cube->SetTranslation(cube2);
			cube->Draw(m_LightShader);
			m_LightShader->UnBind();

			RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::ALWAYS, 0);
			m_FlatShader->Bind();
			m_FlatShader->SetMat4("u_ViewProjection", camProj * viewMatrix,ShaderType::VS);	
			m_FlatShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
			sphere->SetTranslation(LightPosition);
			sphere->SetMaterial(BasicMaterial);
			sphere->SetScale({ 1.0f,1.0f,1.0f });
			sphere->Draw(m_FlatShader);
			m_FlatShader->UnBind();

			//Reflect
			m_ReflectShader->Bind();
			m_ReflectShader->SetMat4("u_ViewProjection", camProj* viewMatrix, ShaderType::VS);
			m_ReflectShader->SetFloat3("viewPosition", tc, ShaderType::PS);
			sphere->SetTranslation(ReflectObjPos);
			sphere->SetScale({ 2.0f,2.0f,2.0f });
			sphere->SetMaterial(ReflectMaterial);
			sphere->Draw(m_ReflectShader);
			//bagPack->SetTranslation(backpackPos);
			//bagPack->SetScale({ 0.5f, 0.5f, 0.5f });
			//bagPack->GetMaterial() = ReflectMaterial;
			//bagPack->Draw(m_ReflectShader);
			m_ReflectShader->UnBind();

			// last drawing for ealry depth test skybox
			RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
			RenderCommand::SetCullMode(CullMode::Front);
			m_SkyBoxShader->Bind();
			glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));
			m_SkyBoxShader->SetMat4("u_ViewProjection", camProj* view, ShaderType::VS);
			RenderCommand::SetDepthWriteMask(DEPTH_WRITE_MASK::MASK_ZERO);
			cubeMap->Draw(m_SkyBoxShader);
			RenderCommand::SetDepthWriteMask(DEPTH_WRITE_MASK::MASK_ALL);
			m_SkyBoxShader->UnBind();
			RenderCommand::SetCullMode(CullMode::Back);

			m_FlatShader->Bind();
			RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::NOT_EQUAL, 1);
			RenderCommand::SetStencilWriteMask(0x00);
			RenderCommand::SetDepthTest(false);
			m_FlatShader->SetFloat4("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), ShaderType::PS);
			cube->SetMaterial(BasicMaterial);
			cube->SetScale({ 0.6f, 0.6f, 0.6f });
			cube->SetTranslation(cube1);
			cube->Draw(m_FlatShader);
			cube->SetTranslation(cube2);
			cube->Draw(m_FlatShader);
			RenderCommand::SetDepthTest(true);
			RenderCommand::SetStencilWriteMask(0xFF);
			RenderCommand::SetStencilTest(false);
			m_FlatShader->UnBind();

			

			framebuffer->UnBind();
			RenderCommand::SetDefaultFrameBuffer();

			RenderCommand::SetBlend(false);
			{
				m_ScreenShader->Bind();
				RenderCommand::SetDepthTest(false);
				m_quad->Bind();
				framebuffer->BindColorTexture(0, 0);
				//woodFloor.GetTexture(Material::MaterialType::Diffuse)->Bind(0);
				RenderCommand::DrawIndexed(m_quad);
				m_ScreenShader->UnBind();
				framebuffer->UnBindTexture();
			}
			
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
		ImGui::DragFloat3("floor", glm::value_ptr(floor), 0.1f);
		ImGui::DragFloat3("backpack", glm::value_ptr(backpackPos), 0.1f);
		ImGui::DragFloat3("obj", glm::value_ptr(ReflectObjPos), 0.1f);
		ImGui::DragFloat3("light", glm::value_ptr(LightPosition), 0.1f);
		ImGui::End();

		sphere->ImguiRender("Spehere 1");

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
		FrameBufferSpecification spec = framebuffer->GetSpecification();
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		if (width>0.0f && height>0.0f && (spec.Width != width || spec.Height != height))
		{
			framebuffer->Resize(width, height);
		}

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
