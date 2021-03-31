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

		//Light
		//glm::vec3 position = glm::vec3(0.5f, 3.0f, -7.2f);
		//dirLight.SetPosition(position);
		//glm::vec3 dir = glm::vec3(0.0f, 0.0f, 0.0f) - position;
		//dirLight.SetDirection(normalize(dir));
		//dirLight.SetDirection({ 0.2f, -1.0f, 0.3f });
		//dirLight.SetDirection({ 0.0f,-0.1f, 0.1f });
		glm::vec3 dir = glm::vec3(0.0f, -2.0f, 0.0f)- glm::vec3(0.5f, 3.0f, -7.2f);
		dirLight.SetDirection(dir);

		//Shader
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			m_LightShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/OmniDirectionalShadowMap/Blinn-phong_Point_Shadow.glsl");
			m_FlatShader = ShaderLibrary::Load("Asset/shaders/glsl/FlatShader.glsl");
			m_ScreenShader = ShaderLibrary::Load("Asset/shaders/glsl/SingleQuad.glsl");
			m_SkyBoxShader = ShaderLibrary::Load("Asset/shaders/glsl/SkyBox.glsl");
			m_ReflectShader = ShaderLibrary::Load("Asset/shaders/glsl/Reflective.glsl");
			m_CubeMapShader = ShaderLibrary::Load("Asset/shaders/glsl/CubeMap.glsl");
			m_ShadowMappingShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/OmniDirectionalShadowMap/PointShadowMapping.glsl");
			m_ScreenDepthShader = ShaderLibrary::Load("Asset/shaders/glsl/ShadowMap/SingleQuadShadow.glsl");
			m_BlinnPhongParallax = ShaderLibrary::Load("Asset/shaders/glsl/ParrallaxMapping/Blinn-phong-Parrallax-PointShadow.glsl");
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			//m_LightShader = ShaderLibrary::Load("LightShader", "BlinnAndPhongPointShadow_VS.cso", 
																//"BlinnAndPhongPointShadow_PS.cso");
			m_LightShader = ShaderLibrary::Load("LightShader", "Asset/shaders/hlsl/ShadowMap/OmniDirectionalShadowMap/BlinnAndPhongPointShadow_VS.hlsl",
				"Asset/shaders/hlsl/ShadowMap/OmniDirectionalShadowMap/BlinnAndPhongPointShadow_PS.hlsl");
			m_ShadowMappingShader = ShaderLibrary::Load("ShadowMapping", "Asset/shaders/hlsl/ShadowMap/OmniDirectionalShadowMap/PointShadowMapping_VS.hlsl", 
																		"Asset/shaders/hlsl/ShadowMap/OmniDirectionalShadowMap/PointShadowMapping_PS.hlsl"
																		, "Asset/shaders/hlsl/ShadowMap/OmniDirectionalShadowMap/PointShadowMapping_GS.hlsl");

			
			m_FlatShader = ShaderLibrary::Load("FlatShader", "Asset/shaders/hlsl/PosNormTcFrag_TransInvTrans.hlsl", "Asset/shaders/hlsl/flatcolor_PS.hlsl");
			m_ScreenShader = ShaderLibrary::Load("QuadShader", "Asset/shaders/hlsl/SingleQuad_VS.hlsl", "Asset/shaders/hlsl/SingleQuad_PS.hlsl");
			m_SkyBoxShader = ShaderLibrary::Load("SkyBoxShader","Asset/shaders/hlsl/SkyBox_VS.hlsl","Asset/shaders/hlsl/SkyBox_PS.hlsl");
			m_ReflectShader = ShaderLibrary::Load("ReflectShader", "Asset/shaders/hlsl/Reflect_VS.hlsl", "Asset/shaders/hlsl/Reflect_PS.hlsl");
			m_CubeMapShader = ShaderLibrary::Load("CubeMapShader","Asset/shaders/hlsl/CubeMap/CubeMap_VS.hlsl", "Asset/shaders/hlsl/CubeMap/CubeMap_PS.hlsl");
			m_ScreenDepthShader = ShaderLibrary::Load("ShadowMapScreen", "Asset/shaders/hlsl/ShadowMap/ScreenShadow_VS.hlsl", "Asset/shaders/hlsl/ShadowMap/ScreenShadow_PS.hlsl");
		}

		//material.SetTexutre("Asset/textures/matrix.jpg", Material::MaterialType::Emission);

		woodFloor = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		grass     = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		window    = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		brick	  = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		Box	      = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		BasicMaterial = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		ReflectMaterial = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);
		CubeMapMat = Material(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.f);

		// wood Texture
		Ref<Texture2D> woodTexture = TextureLibrary::Load("Asset/textures/floor.png");
		// grass Texture
		Ref<Texture2D> grassTexture = TextureLibrary::Load("Asset/textures/grass.png"); 
		// window Texture
		Ref<Texture2D> windowTexture = TextureLibrary::Load("Asset/textures/blending_transparent_window.png");
		// Brickwall Texture
		Ref<Texture2D> brickTexture = TextureLibrary::Load("Asset/textures/bricks2.jpg");
		// Brickwall normalmap
		Ref<Texture2D> brickNormalMap = TextureLibrary::Load("Asset/textures/bricks2_normal.jpg");
		// Box diffuse
		Ref<Texture2D> boxDiffuse = TextureLibrary::Load("Asset/textures/container2.png");
		// Box specular
		Ref<Texture2D> boxSpecular = TextureLibrary::Load("Asset/textures/container2_specular.png");
		// heightMap
		heightMap = TextureLibrary::Load("Asset/textures/bricks2_disp.jpg");

		// Sky Box
		/*std::vector<std::string> imagePath =
		{
			"Asset/textures/skybox/right.jpg",
			"Asset/textures/skybox/left.jpg",
			"Asset/textures/skybox/top.jpg",
			"Asset/textures/skybox/bottom.jpg",
			"Asset/textures/skybox/front.jpg",
			"Asset/textures/skybox/back.jpg",
		};*/
		std::vector<std::string> imagePath =
		{
			"Asset/textures/Lycksele/posx.jpg",
			"Asset/textures/Lycksele/negx.jpg",
			"Asset/textures/Lycksele/posy.jpg",
			"Asset/textures/Lycksele/negy.jpg",
			"Asset/textures/Lycksele/posz.jpg",
			"Asset/textures/Lycksele/negz.jpg",
		};
		Sampler_Desc desc;
		m_cubeTexture = TextureCube::Create(imagePath,desc);

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
		diona = Model::Create("Asset/model/diona/diona.fbx");
		diona->SetTranslation({ 0.0f,0.0f,0.0f });
		muro = Model::Create("Asset/model/muro/muro.obj");
	
		m_LightShader->Bind();
		m_LightShader->SetInt("material.diffuse", 0, ShaderType::PS);
		m_LightShader->SetInt("material.specular", 1, ShaderType::PS);
		m_LightShader->SetInt("material.normal", 2, ShaderType::PS);
		m_LightShader->SetInt("depthMap", 4, ShaderType::PS);

		m_BlinnPhongParallax->Bind();
		m_BlinnPhongParallax->SetInt("material.diffuse", 0, ShaderType::PS);
		m_BlinnPhongParallax->SetInt("material.specular", 1, ShaderType::PS);
		m_BlinnPhongParallax->SetInt("material.normal", 2, ShaderType::PS);
		m_BlinnPhongParallax->SetInt("depthMap", 4, ShaderType::PS);
		m_BlinnPhongParallax->SetInt("heightMap", 5, ShaderType::PS);

		m_ScreenShader->Bind();
		m_ScreenShader->SetInt("screenTexture", 0, ShaderType::PS);

		m_ScreenDepthShader->Bind();
		m_ScreenDepthShader->SetInt("screenTexture", 0, ShaderType::PS);

		m_SkyBoxShader->Bind();
		m_SkyBoxShader->SetInt("skybox", 0, ShaderType::PS);

		m_ReflectShader->Bind();
		m_ReflectShader->SetInt("skybox", 0, ShaderType::PS);

		m_CubeMapShader->Bind();
		m_CubeMapShader->SetInt("skybox", 0, ShaderType::PS);

		m_ShadowMappingShader->Bind();
		m_ShadowMappingShader->SetInt("diffuseTex", 0, ShaderType::PS);

		face = CreateRef<Face>(glm::vec3(0.0f, -3.0f, 0.0f),m_LightShader,woodFloor,1);
		face->SetScale({ 5.0f, 5.0f, 5.0f });
		sphere = CreateRef<Sphere>(glm::vec3(0.0f,-2.7f,-0.9f), m_FlatShader, 0.1f);
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
		cube3 = glm::vec3(0.0f, -2.0f, -1.0f);
		
		window1 = glm::vec3(0.9f, -2.75f, -0.7f);
		window2 = glm::vec3(0.3f, -2.75f, 0.3f);
		window3 = glm::vec3(0.5f, -2.75f, -0.1f);

		backpackPos = glm::vec3(-1.f, -3.0f, 0.5f);
		backpackRot = glm::vec3(0.0f);
		ReflectObjPos = glm::vec3(0.5f, -2.0f, -0.5f);

		FrameBufferSpecification spec;
		spec.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8},
							 {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8},
							 {FramebufferUsage::Depth_Stencil ,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8} };
		spec.Width = 1600;
		spec.Height = 900;
		framebuffer = FrameBuffer::Create(spec);

		FrameBufferSpecification spec2;
		spec2.Attachments = {
							  {FramebufferUsage::Depth,TextureType::TextureCube ,TextureFormat::DEPTH32} };
		spec2.Width  = 2048;
		spec2.Height = 2048;
		DepthFrameBuffer = FrameBuffer::Create(spec2);

		FrameBufferSpecification spec3;
		spec3.Attachments = { {FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8} };
		spec3.Width = 1024;
		spec3.Height = 1024;
		screenframeBuffer = FrameBuffer::Create(spec3);

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

		desc.MIN = Filtering::POINT;
		desc.MAG = Filtering::POINT;
		desc.MIP = Filtering::NONE;
		
		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.addressW = WrapingMode::CLAMP;

		defaultSampler = SamplerState::Create(desc);

		desc.MIN = Filtering::LINEAR;
		desc.MAG = Filtering::LINEAR;
		desc.MIP = Filtering::NONE;

		desc.addressU = WrapingMode::CLAMP;
		desc.addressV = WrapingMode::CLAMP;
		desc.addressW = WrapingMode::CLAMP;

		desc.mode = FilterMode::COMPARISON;
		desc.comparisonFunc = COMPARISON_FUNC::LESS_EQUAL;

		shadowSampler = SamplerState::Create(desc);

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
			RenderCommand::SetDepthTest(true);
			RenderCommand::SetBlend(true);
			RenderCommand::SetBlendFunc(BlendFunc::BLEND_SRC_ALPHA, BlendFunc::BLEND_INV_SRC_ALPHA, BlendFunc::BLEND_ONE, BlendFunc::BLEND_ZERO);
			RenderCommand::SetBlendOp(BlendOp::BLEND_ADD, BlendOp::BLEND_ADD);

			
			//RenderCommand::SetCullMode(CullMode::Back);

			const glm::mat4& camProj = m_Camera.GetComponent<CameraComponent>().Camera.GetProjection();
			auto& tc = m_Camera.GetComponent<TransformComponent>().Translation;

			float near_plane = 0.0001f, far_plane = 100.0f;
			const glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
			//const glm::mat4 lightProjection = camProj;
			glm::mat4 lightView = glm::lookAt(glm::vec3(0.5f, 3.0f, -7.2f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// DepthMap Rendering
			DepthFrameBuffer->Bind();
			DepthFrameBuffer->Clear();

			// shader
			float nearplane = 0.01f;
			float farplane = 50.0f;
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearplane, farplane);

			std::vector<glm::mat4> shadowTransforms;
			if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			{
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
			}
			else
			{
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
				shadowTransforms.push_back(shadowProj *
					glm::lookAt(LightPosition, LightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0)));
			}
			
			glm::vec4 vector1 = { 0.0437f,-0.2996f,0.9f,1.0f};
			glm::vec4 vector2 = { 4.0f,1.f,2.0f,1.0f };

			vector1  += glm::vec4(LightPosition, 0.0f);
			vector1 = shadowTransforms[4] * vector1;
			vector2 = shadowProj * vector2;

			m_ShadowMappingShader->Bind();
			for (int i = 0; i < 6; ++i)
				m_ShadowMappingShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i], ShaderType::GS);
			m_ShadowMappingShader->SetFloat3("lightPos", LightPosition, ShaderType::PS);
			m_ShadowMappingShader->SetFloat("far_plane", farplane, ShaderType::PS);

			RenderCommand::SetDepthBias(depthBias);
			RenderCommand::SetSlopeScaledDepthBias(slopeBias);
			RenderCommand::SetDepthBiasClamp(depthClamp);
			RenderScene(lightProjection, lightView, { 0.0f,0.0f,0.0f }, m_ShadowMappingShader);
			m_ShadowMappingShader->UnBind();

			DepthFrameBuffer->UnBind();


			// new framebuffer
			framebuffer->Bind();
			framebuffer->Clear();

			m_LightShader->Bind();

			RenderCommand::SetDepthBias(0);
			RenderCommand::SetSlopeScaledDepthBias(0.0f);
			RenderCommand::SetDepthBiasClamp(0.0f);

			m_LightShader->SetFloat("far_plane", farplane, ShaderType::PS);
			m_LightShader->SetFloat("near_plane", nearplane, ShaderType::PS);
			DepthFrameBuffer->BindDepthTexture(4);
			//shadowSampler->Bind(4);

			shadowSampler->Bind(4);
			RenderLightObj(camProj, viewMatrix, tc);
			m_LightShader->UnBind();
			m_BlinnPhongParallax->Bind();
			DepthFrameBuffer->BindDepthTexture(4);
			heightMap->Bind(5);

			m_BlinnPhongParallax->SetFloat("far_plane", 50.0f, ShaderType::PS);
			m_BlinnPhongParallax->SetFloat("near_plane", 0.01f, ShaderType::PS);
			m_BlinnPhongParallax->SetFloat("height_scale", 0.1f, ShaderType::PS);
			m_BlinnPhongParallax->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			m_BlinnPhongParallax->SetFloat3("viewPosition", tc, ShaderType::VS);
			m_BlinnPhongParallax->SetBool("gamma", gamma, ShaderType::PS);

			// Point Light 1
			m_BlinnPhongParallax->SetFloat3("pointLight.position", LightPosition, ShaderType::PS);
			m_BlinnPhongParallax->SetFloat3("pointLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f), ShaderType::PS);
			m_BlinnPhongParallax->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
			m_BlinnPhongParallax->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);

			m_BlinnPhongParallax->SetFloat("pointLight.constant", constant, ShaderType::PS);
			m_BlinnPhongParallax->SetFloat("pointLight.Linear", Linear, ShaderType::PS);
			m_BlinnPhongParallax->SetFloat("pointLight.quadratic", quadratic, ShaderType::PS);

			face->SetMaterial(brick);
			face->SetTranslation(brickwall);
			face->SetRotation(floorRot);
			face->SetScale({ 5.0f,3.0f,1.0f });
			face->MaterialBind();
			face->Draw(m_BlinnPhongParallax);

			m_BlinnPhongParallax->UnBind();

			RenderNonLightObj(camProj, viewMatrix, tc);
			DepthFrameBuffer->UnBindTexture();
			//shadowSampler->UnBind();

			//Reflect
			//RenderCommand::SetBlend(false);
			m_ReflectShader->Bind();
			m_ReflectShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			m_ReflectShader->SetFloat3("viewPosition", tc, ShaderType::PS);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), ReflectObjPos) * glm::scale(glm::mat4(1.0f), { 2.0f,2.0f,2.0f });
			m_ReflectShader->SetMat4("u_Transform", transform, ShaderType::VS);
			m_ReflectShader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
			ReflectMaterial.Bind(0, Material::MaterialType::Diffuse);
			m_ReflectShader->UpdateBuffer();
			sphere->GetVertexArray()->Bind();
			RenderCommand::DrawIndexed(sphere->GetVertexArray());
			//bagPack->SetTranslation(backpackPos);
			//bagPack->SetRotation(backpackRot);
			//bagPack->SetScale({ 0.5f, 0.5f, 0.5f });
			//bagPack->GetMaterial() = ReflectMaterial;
			//bagPack->Draw(m_ReflectShader);
			m_ReflectShader->UnBind();

			RenderCommand::SetCullMode(CullMode::Front);
			m_CubeMapShader->Bind();
			m_CubeMapShader->SetMat4("u_ViewProjection", camProj * viewMatrix, ShaderType::VS);
			transform = glm::translate(glm::mat4(1.0f), { -3.0f,-2.0f,-1.0f }) * 
				glm::toMat4(glm::quat({ 0.0f,0.0f,0.0f })) * glm::scale(glm::mat4(1.0f), { 1.0f,1.0f,1.0f });
			m_CubeMapShader->SetMat4("u_Transform", transform, ShaderType::VS);
			DepthFrameBuffer->BindDepthTexture(0);
			//ReflectMaterial.Bind(0, Material::MaterialType::Diffuse);
			RenderCommand::SetCullMode(CullMode::None);
			cube->GetVertexArray()->Bind();
			RenderCommand::DrawIndexed(cube->GetVertexArray());
			m_CubeMapShader->UpdateBuffer();
			m_CubeMapShader->UnBind();

			RenderSkyObj(camProj, viewMatrix, tc);

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
		ImGui::DragFloat3("backpackRot", glm::value_ptr(backpackRot), 0.1f);
		ImGui::DragFloat3("obj", glm::value_ptr(ReflectObjPos), 0.1f);
		ImGui::DragFloat3("light", glm::value_ptr(LightPosition), 0.1f);
		
		ImGui::DragInt("Depth Bias", &depthBias);
		ImGui::DragFloat("SlopeDepthBias", &slopeBias, 0.1f);
		ImGui::DragFloat("DepthClamp", &depthClamp, 0.1f);

		ImGui::End();
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
			screenframeBuffer->Resize(width, height);
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

	void Sandbox2D::RenderSkyObj(const glm::mat4& proj,const glm::mat4& view,const glm::vec3& camPos)
	{	
			// last drawing for ealry depth test skybox
			RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
			RenderCommand::SetCullMode(CullMode::Front);
			m_SkyBoxShader->Bind();
			glm::mat4 view3x3 = glm::mat4(glm::mat3(view));
			m_SkyBoxShader->SetMat4("u_ViewProjection", proj* view3x3, ShaderType::VS);
			RenderCommand::SetDepthWriteMask(DEPTH_WRITE_MASK::MASK_ZERO);
			cubeMap->Draw(m_SkyBoxShader);
			RenderCommand::SetDepthWriteMask(DEPTH_WRITE_MASK::MASK_ALL);
			m_SkyBoxShader->UnBind();
			RenderCommand::SetCullMode(CullMode::Back);
	}

	void Sandbox2D::RenderLightObj(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos)
	{
		m_LightShader->SetMat4("u_ViewProjection", proj * view, ShaderType::VS);
		m_LightShader->SetFloat3("viewPosition", camPos, ShaderType::VS);
		m_LightShader->SetBool("gamma", gamma, ShaderType::PS);

		// Point Light 1
		m_LightShader->SetFloat3("pointLight.position", LightPosition, ShaderType::PS);
		m_LightShader->SetFloat3("pointLight.ambient", glm::vec3(0.2f,0.2f,0.2f), ShaderType::PS);
		m_LightShader->SetFloat3("pointLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);
		m_LightShader->SetFloat3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f), ShaderType::PS);

		m_LightShader->SetFloat("pointLight.constant", constant, ShaderType::PS);
		m_LightShader->SetFloat("pointLight.Linear", Linear, ShaderType::PS);
		m_LightShader->SetFloat("pointLight.quadratic", quadratic, ShaderType::PS);

		face->SetMaterial(woodFloor);
		face->SetTranslation(floor);
		face->SetScale({ 20.0f,20.0f,20.0f });
		face->SetRotation({ 1.6f,0.0f,0.0f });
		face->MaterialBind();
		face->Draw(m_LightShader);

		face->SetMaterial(grass);
		face->SetScale({ 0.5f,0.5f,0.5f });
		face->SetRotation({ 0.0f,0.0f,0.0f });
		face->SetTranslation(grass1);
		face->MaterialBind();
		face->Draw(m_LightShader);
		face->SetTranslation(grass2);
		face->MaterialBind();
		face->Draw(m_LightShader);
		face->SetTranslation(grass3);
		face->MaterialBind();
		face->Draw(m_LightShader);
		diona->SetTranslation(backpackPos);
		diona->SetScale({ 0.1f, 0.1f, 0.1f });
		diona->Draw(m_LightShader);
		muro->SetTranslation({ 2.0f, -3.0f, 0.0f });
		muro->SetScale({ 1.0f,1.0f,1.0f });
		muro->Draw(m_LightShader);

		/*RenderCommand::SetStencilTest(true);
		RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::ALWAYS, 1);
		RenderCommand::SetFrontStencilOp(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::REPLACE);
		RenderCommand::SetStencilWriteMask(0xFF);*/
		cube->SetMaterial(Box);
		cube->SetScale({ 0.5f, 0.5f, 0.5f });
		cube->SetTranslation(cube1);
		cube->Draw(m_LightShader);
		cube->SetMaterial(brick);
		cube->SetTranslation(cube2);
		cube->Draw(m_LightShader);
	}

	void Sandbox2D::RenderNonLightObj(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos)
	{
		//RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::ALWAYS, 0);
		m_FlatShader->Bind();
		m_FlatShader->SetMat4("u_ViewProjection", proj * view, ShaderType::VS);
		m_FlatShader->SetFloat4("u_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ShaderType::PS);
		sphere->SetTranslation(LightPosition);
		sphere->SetMaterial(BasicMaterial);
		sphere->SetScale({ 1.0f,1.0f,1.0f });
		sphere->Draw(m_FlatShader);
		m_FlatShader->UnBind();

	}

	void Sandbox2D::RenderScene(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& camPos, const Ref<Shader>& shader)
	{
		shader->Bind();

		face->SetMaterial(woodFloor);
		face->SetTranslation(floor);
		face->SetScale({ 20.0f,20.0f,20.0f });
		face->SetRotation({ 1.6f,0.0f,0.0f });
		face->Draw(shader);
		face->SetMaterial(brick);
		face->SetTranslation(brickwall);
		face->SetRotation(floorRot);
		face->SetScale({ 5.0f,3.0f,1.0f });
		face->MaterialBind();
		face->Draw(shader);

		face->SetMaterial(grass);
		face->SetScale({ 0.5f,0.5f,0.5f });
		face->SetRotation({ 0.0f,0.0f,0.0f });
		face->SetTranslation(grass1);
		face->MaterialBind();
		face->Draw(shader);
		face->SetTranslation(grass2);
		face->MaterialBind();
		face->Draw(shader);
		face->SetTranslation(grass3);
		face->MaterialBind();
		face->Draw(shader);

		diona->SetTranslation(backpackPos);
		diona->SetScale({ 0.1f, 0.1f, 0.1f });
		diona->Draw(shader);

		muro->SetTranslation({ 2.0f, -3.0f, 0.0f });
		muro->SetScale({ 1.0f,1.0f,1.0f });
		muro->Draw(shader);

		/*RenderCommand::SetStencilTest(true);
		RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::ALWAYS, 1);
		RenderCommand::SetFrontStencilOp(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::REPLACE);
		RenderCommand::SetStencilWriteMask(0xFF);*/
		cube->SetMaterial(Box);
		cube->SetScale({ 0.5f, 0.5f, 0.5f });
		cube->SetTranslation(cube1);
		cube->Draw(shader);
		cube->SetMaterial(brick);
		cube->SetTranslation(cube2);
		cube->Draw(shader);

		sphere->SetTranslation(LightPosition);
		sphere->SetMaterial(BasicMaterial);
		sphere->SetScale({ 1.0f,1.0f,1.0f });
		sphere->Draw(shader);
		shader->UnBind();
	}

}

