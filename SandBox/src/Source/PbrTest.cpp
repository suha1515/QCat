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
		width = 1600;
		height = 900;
		m_ActiveScene = CreateRef<Scene>();
		m_Camera = m_ActiveScene->CreateEntity("Camera");

		auto& tc = m_Camera.GetComponent<TransformComponent>();
		tc.Translation = { 0.0f,0.0f,-2.0f };

		auto& camera = m_Camera.AddComponent<CameraComponent>();
		camera.Camera.SetViewportSize(1600.0f, 900.0f);
		camera.Camera.SetPerspective(1, 0.01f, 100.0f);

		//PBRshader = ShaderLibrary::Load("Blinn-Phong", "Asset/shaders/glsl/VertexShader/World_TexNormFragTbn_Out.glsl",
		//	"Asset/shaders/glsl/PixelShader/Blinn-phong.glsl");
		Flatcolor = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/FlatColor.hlsl" : "Asset/shaders/glsl/FlatColor.glsl");
		PBRshader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/PBR_PointLight.hlsl" : "Asset/shaders/glsl/PBR/PBR_PointLight.glsl");
		HdrToCube = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/HDRtoCube.hlsl": "Asset/shaders/glsl/PBR/HDRtoCube.glsl");
		HdrCubeMap = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/MakeHDRCubeMap.hlsl" : "Asset/shaders/glsl/PBR/MakeHDRCubeMap.glsl");
		IrradianceMap= ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/IrradianceConvolution.hlsl" : "Asset/shaders/glsl/PBR/IrradianceConvolution.glsl");
		prefilter = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/prefilter.hlsl" : "Asset/shaders/glsl/PBR/prefilter.glsl");
		//helmet = Model::Create("Asset/model/gun2/gun2.fbx");
		//helmet->SetTranslation({ 0.0f,0.0f,-1.0f });
		//helmet->SetScale({ 0.01f,0.01f,0.01f });
		PBRshader->Bind();
		PBRshader->SetInt("material.albedoMap", 0, ShaderType::PS);
		PBRshader->SetInt("material.normalMap", 1, ShaderType::PS);
		PBRshader->SetInt("material.metallicMap", 2, ShaderType::PS);
		PBRshader->SetInt("material.roughnessMap", 3, ShaderType::PS);
		PBRshader->SetInt("material.aoMap", 4, ShaderType::PS);
		PBRshader->SetInt("irradianceMap", 5, ShaderType::PS);

		Sampler_Desc desc;
		HdrToCube->Bind();
		HdrToCube->SetInt("equirectangularMap", 0, ShaderType::PS);

		HdrCubeMap->Bind();
		HdrCubeMap->SetInt("environmentMap", 0, ShaderType::PS);

		IrradianceMap->Bind();
		IrradianceMap->SetInt("envMap", 0, ShaderType::PS);

		prefilter->Bind();
		prefilter->SetInt("environmentMap", 0, ShaderType::PS);
	
		//HDRImage = TextureLibrary::Load("Asset/textures/HdrImage/Arches_E_PineTree/Arches_E_PineTree_8k.jpg", desc, 1, 1, RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? false : true);
		HDRImage = TextureLibrary::Load("Asset/textures/HdrImage/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", desc, 1, 1, RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? false : true);
		//pbrmat.SetTexture("Asset/textures/rusted_iron/albedo.png", desc, Material::TextureType::Diffuse);
		//pbrmat.SetTexture("Asset/textures/rusted_iron/normal.png", desc, Material::TextureType::Normal);
		//pbrmat.SetTexture("Asset/textures/rusted_iron/metallic.png", desc, Material::TextureType::Metallic);
		//pbrmat.SetTexture("Asset/textures/rusted_iron/roughness.png", desc, Material::TextureType::Roughness);

		pbrmat.metallic = 0.0f;
		pbrmat.roughness = 0.5f;
		pbrmat.diffuse = { 0.5f,0.0f,0.0f };
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
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		//Framebuffer
		FrameBufferSpecification spec;
		spec.Attachments = { {FramebufferUsage::Color, TextureType::TextureCube, TextureFormat::RGB16_Float},
							 {FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8} };
		spec.Width = 512;
		spec.Height = 512;
		//cubeMapPass = FrameBuffer::Create(spec);
		AttachmentSpecification spec2 = { { FramebufferUsage::Color,TextureType::TextureCube,TextureFormat::RGB16_Float,"ColorBuffer1" },
										  { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" } };
		cubeMapPass = FrameBufferEx::Create(spec2);

		float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? -1.0f : 1.0f;
		captureProjection;
		captureViews[6];
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			captureProjection = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.01f, 10.0f);
			captureViews[0] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[1] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[2] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f * bias));
			captureViews[3] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f * bias));
			captureViews[4] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[5] = glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
		}
		else
		{
			captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 10.0f);
			captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f * bias));
			captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f * bias));
			captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
			captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f * bias, 0.0f));
		}

		// Hdr Cubemap pass
		Texture_Desc texDesc;
		texDesc.Width = 512;
		texDesc.Height = 512;
		texDesc.MipLevels = 1;
		texDesc.SampleCount = 1;
		Sampler_Desc smpDesc;
		smpDesc.addressU = WrapingMode::CLAMP;
		smpDesc.addressV = WrapingMode::CLAMP;
		smpDesc.MIN = Filtering::LINEAR;
		smpDesc.MAG = Filtering::LINEAR;
		smpDesc.MIP = Filtering::NONE;
		

		IrradianceCubeMapTexture = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 32, 32);
		smpDesc.MIP = Filtering::LINEAR;
		HdrCubeMapTexture = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 512, 512,8);
		PrefilterMap = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 128, 128,5);
		PrefilterMap->GenerateMipMap();
		smpDesc.MIP = Filtering::NONE;
		cubeMapPass->InitializeTexture("DepthBuffer", texDesc, smpDesc);
		cubeMapPass->Bind();
		cubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		cubeMapPass->AttachTexture(HdrCubeMapTexture, AttachmentType::Color_0, TextureType::TextureCube_PositiveX, 0);
		cubeMapPass->UnBind();

		RenderCommand::SetCullMode(CullMode::None);

		cubeMapPass->Bind();
		RenderCommand::SetViewport(0, 0, 512, 512);
		HdrToCube->Bind();
		HdrToCube->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		for (int i = 0; i < 6; ++i)
		{
			int index = (int)TextureType::TextureCube_PositiveX + i;
			cubeMapPass->AttachTexture(HdrCubeMapTexture, AttachmentType::Color_0,(TextureType)index, 0);
			cubeMapPass->Clear();
			HdrToCube->SetMat4("u_View", captureViews[i], ShaderType::VS);
			HDRImage->Bind(0);
			HdrToCube->UpdateBuffer();
			cube->Draw();
		}
		HdrToCube->UnBind();
		cubeMapPass->UnBind();

		HdrCubeMapTexture->GenerateMipMap();

		RenderCommand::SetCullMode(CullMode::None);
		cubeMapPass->Bind();
		RenderCommand::SetViewport(0, 0, 32, 32);
		cubeMapPass->GetTexture("DepthBuffer")->SetSize(32, 32);
		cubeMapPass->DetachTexture(AttachmentType::Color_0);
		cubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		cubeMapPass->AttachTexture(IrradianceCubeMapTexture, AttachmentType::Color_0, TextureType::TextureCube_PositiveX, 0);

		IrradianceMap->Bind();
		IrradianceMap->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		for (int i = 0; i < 6; ++i)
		{
			int index = (int)TextureType::TextureCube_PositiveX + i;
			cubeMapPass->AttachTexture(IrradianceCubeMapTexture, AttachmentType::Color_0, (TextureType)index, 0);
			cubeMapPass->Clear();

			IrradianceMap->SetMat4("u_View", captureViews[i], ShaderType::VS);
			IrradianceMap->UpdateBuffer();
			HdrCubeMapTexture->Bind(0);
			cube->Draw();
		}
		IrradianceMap->UnBind();

		cubeMapPass->UnBind();

		//PreFilterShader
		cubeMapPass->Bind();

		prefilter->Bind();
		prefilter->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		unsigned int maxMipLevels = 5;
		for (int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);

			cubeMapPass->GetTexture("DepthBuffer")->SetSize(mipWidth, mipHeight);
			cubeMapPass->DetachTexture(AttachmentType::Color_0);
			cubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
			RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);

			float roughtness = (float)mip / (float)(maxMipLevels - 1);
			prefilter->SetFloat("roughness", roughtness, ShaderType::PS);
			for (int i = 0; i < 6; ++i)
			{
				HdrCubeMapTexture->Bind(0);
				int index = (int)TextureType::TextureCube_PositiveX + i;
				prefilter->SetMat4("u_View", captureViews[i], ShaderType::VS);
				cubeMapPass->AttachTexture(PrefilterMap, AttachmentType::Color_0, (TextureType)index, mip);
				cubeMapPass->Clear();
				prefilter->UpdateBuffer();
				cube->Draw();
			}
		}
		prefilter->UnBind();
		cubeMapPass->UnBind();
		
	}

	void PbrTest::OnDetach()
	{
	}

	void PbrTest::OnUpdate(Timestep ts)
	{
		RenderCommand::SetDefaultFrameBuffer();
		RenderCommand::SetViewport(0, 0, width, height);
		RenderCommand::SetCullMode(CullMode::Back);
		QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		QCat::RenderCommand::Clear();

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
		pbrmat.diffuse = { 0.5f,0.0f,0.0f };
		for (int i = 0; i < 7; ++i)
		{
			pbrmat.metallic = (float)i / (float)7;
			for (int j = 0; j < 7; ++j)
			{
				//irradiance Map
				IrradianceCubeMapTexture->Bind(5);
				float roughness = glm::clamp((float)j / (float)7, 0.05f, 1.0f);
				pbrmat.roughness = roughness;
				sphere->SetTranslation(glm::vec3(-1.05f+j*0.3f, 1.05f-i*0.3f, 0.0f));
				glm::mat4 transform = sphere->GetTransform();
				PBRshader->SetMat4("u_Transform", transform, ShaderType::VS);
				PBRshader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
				// material
				if (pbrmat.IsThereTexture(Material::TextureType::Normal))
					PBRshader->SetBool("material.IsNormalMap", true, ShaderType::PS);
				else
					PBRshader->SetBool("material.IsNormalMap", false, ShaderType::PS);
				
				if (pbrmat.IsThereTexture(Material::TextureType::Diffuse))
					PBRshader->SetBool("material.IsAlbedoMap", true, ShaderType::PS);
				else
				{
					PBRshader->SetBool("material.IsAlbedoMap", false, ShaderType::PS);
					PBRshader->SetFloat3("material.albedo", pbrmat.diffuse, ShaderType::PS);
				}
				if (pbrmat.IsThereTexture(Material::TextureType::Metallic))
					PBRshader->SetBool("material.IsMetallicMap", true, ShaderType::PS);
				else
				{
					PBRshader->SetBool("material.IsMetallicMap", false, ShaderType::PS);
					PBRshader->SetFloat("material.metallic", pbrmat.metallic, ShaderType::PS);
				}
				if (pbrmat.IsThereTexture(Material::TextureType::Roughness))
					PBRshader->SetBool("material.IsRoughnessMap", true, ShaderType::PS);
				else
				{
					PBRshader->SetBool("material.IsRoughnessMap", false, ShaderType::PS);
					PBRshader->SetFloat("material.roughness", pbrmat.roughness, ShaderType::PS);
				}
				if (pbrmat.IsThereTexture(Material::TextureType::AmbientOcclusion))
					PBRshader->SetBool("material.IsAoMap", true, ShaderType::PS);
				else
					PBRshader->SetBool("material.IsAoMap", false, ShaderType::PS);
					PBRshader->SetFloat("material.ambientocclusion", pbrmat.ao, ShaderType::PS);
			

				PBRshader->UpdateBuffer();
				pbrmat.Bind(0, Material::TextureType::Diffuse);
				pbrmat.Bind(1, Material::TextureType::Normal);
				pbrmat.Bind(2, Material::TextureType::Metallic);
				pbrmat.Bind(3, Material::TextureType::Roughness);
				pbrmat.Bind(4, Material::TextureType::AmbientOcclusion);
				sphere->Draw(PBRshader);
			}
		}
		{
		/*	cube->SetTranslation({ 0.0f,0.0f,-2.0f });
			cube->SetScale({ 0.5f,0.5f,0.5f });
			glm::mat4 cubeTransform = cube->GetTransform();
			pbrmat.metallic = 0.2f;
			pbrmat.roughness = 0.5f;
			pbrmat.diffuse = { 0.0f,0.5f,0.0f };
			PBRshader->SetMat4("u_Transform", cubeTransform, ShaderType::VS);
			PBRshader->SetMat4("u_invTransform", glm::inverse(cubeTransform), ShaderType::VS);

			PBRshader->SetBool("material.IsAlbedoMap", false, ShaderType::PS);
			PBRshader->SetBool("material.IsNormalMap", false, ShaderType::PS);
			PBRshader->SetBool("material.IsMetallicMap", false, ShaderType::PS);
			PBRshader->SetBool("material.IsRoughnessMap", false, ShaderType::PS);
			PBRshader->SetBool("material.IsAoMap", false, ShaderType::PS);

			PBRshader->SetFloat3("material.albedo", pbrmat.diffuse, ShaderType::PS);
			PBRshader->SetFloat("material.metallic", pbrmat.metallic, ShaderType::PS);
			PBRshader->SetFloat("material.roughness", pbrmat.roughness, ShaderType::PS);
			PBRshader->SetFloat("material.ambientocclusion", pbrmat.ao, ShaderType::PS);

			PBRshader->UpdateBuffer();
			cube->Draw();*/
		}
		//std::vector<Mesh>& meshes = helmet->GetMeshes();
		
		//helmet->SetRotation(rotation);
		//for (auto& mesh : meshes)
		//{
		//	glm::mat4 transform = helmet->GetTransform() * mesh.GetTransform();
		//	PBRshader->SetMat4("u_Transform", transform, ShaderType::VS);
		//	PBRshader->SetMat4("u_invTransform", glm::inverse(transform), ShaderType::VS);
		//	Material& helmetMat = mesh.GetMaterial();
		//	// material
		//	if (helmetMat.IsThereTexture(Material::TextureType::Normal))
		//		PBRshader->SetBool("material.IsNormalMap", true, ShaderType::PS);
		//	else
		//		PBRshader->SetBool("material.IsNormalMap", false, ShaderType::PS);
		//	if (helmetMat.IsThereTexture(Material::TextureType::Diffuse))
		//		PBRshader->SetBool("material.IsAlbedoMap", true, ShaderType::PS);
		//	else
		//	{
		//		PBRshader->SetBool("material.IsAlbedoMap", false, ShaderType::PS);
		//		PBRshader->SetFloat3("material.albedo", helmetMat.diffuse, ShaderType::PS);
		//	}
		//	if (helmetMat.IsThereTexture(Material::TextureType::Metallic))
		//		PBRshader->SetBool("material.IsMetallicMap", true, ShaderType::PS);
		//	else
		//	{
		//		PBRshader->SetBool("material.IsMetallicMap", false, ShaderType::PS);
		//		PBRshader->SetFloat("material.metallic", helmetMat.metallic, ShaderType::PS);
		//	}
		//	if (helmetMat.IsThereTexture(Material::TextureType::Roughness))
		//		PBRshader->SetBool("material.IsRoughnessMap", true, ShaderType::PS);
		//	else
		//	{
		//		PBRshader->SetBool("material.IsRoughnessMap", false, ShaderType::PS);
		//		PBRshader->SetFloat("material.roughness", helmetMat.roughness, ShaderType::PS);
		//	}
		//	if (helmetMat.IsThereTexture(Material::TextureType::AmbientOcclusion))
		//		PBRshader->SetBool("material.IsAoMap", true, ShaderType::PS);
		//	else
		//		PBRshader->SetBool("material.IsAoMap", false, ShaderType::PS);
		//	PBRshader->SetFloat("material.ambientocclusion", helmetMat.ao, ShaderType::PS);

		//	PBRshader->UpdateBuffer();		
		//	
		//	helmetMat.Bind(0, Material::TextureType::Diffuse);
		//	helmetMat.Bind(1, Material::TextureType::Normal);
		//	helmetMat.Bind(2, Material::TextureType::Metallic);
		//	helmetMat.Bind(3, Material::TextureType::Roughness);
		//	helmetMat.Bind(4, Material::TextureType::AmbientOcclusion);
		//	mesh.Draw();
		//}
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
	

		//Draw HDR skyBox
		RenderCommand::SetCullMode(CullMode::None);
		RenderCommand::SetDepthFunc(COMPARISON_FUNC::LESS_EQUAL);
		{
			HdrCubeMap->Bind();
			HdrCubeMap->SetMat4("u_Projection", camProj, ShaderType::VS);
			HdrCubeMap->SetMat4("u_View", viewMatrix, ShaderType::VS);
			HdrCubeMap->UpdateBuffer();
			//HdrCubeMapTexture->Bind(0);
			PrefilterMap->Bind(0);
			//IrradianceCubeMapTexture->Bind(0);
			//cubeMapPass->GetTexture("ColorBuffer1")->Bind(0);

			cube->Draw();
			HdrCubeMap->UnBind();
		}

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
		ImGui::DragFloat3("helmet rot", glm::value_ptr(rotation), 0.1f);
		ImGui::DragInt("num", &num);
		ImGui::End();
	}

	void PbrTest::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
	}

	bool PbrTest::OnWindowResize(WindowResizeEvent& e)
	{
		 width = e.GetWidth();
		 height = e.GetHeight();

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
			viewMatrix = glm::lookAt(tc, tc + cameraFront, cameraUp);
	}
}

