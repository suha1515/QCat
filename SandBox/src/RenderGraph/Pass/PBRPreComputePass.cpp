#include "PBRPreComputePass.h"

namespace QCat
{
	PBRPreComputePass::PBRPreComputePass(unsigned int level, const std::string& name)
	{
		this->level = level;
		this->name = name;

		RegisterInput(TextureInput::Make("HdrImage", m_HdrImage));
		RegisterOutput(TextureOutput::Make("IrradianceCubeMap", m_IrradianceCubeMap));
		RegisterOutput(TextureOutput::Make("HdrCubeMap", m_HdrCubeMap));
		RegisterOutput(TextureOutput::Make("BRDFLutTexture", m_BRDFLutTexture));
		RegisterOutput(TextureOutput::Make("PrefilterMap", m_PrefilterMap));
	}

	void PBRPreComputePass::Initialize()
	{
		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

		m_BRDFLutShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/BRDF2D.hlsl" : "Asset/shaders/glsl/PBR/BRDF2D.glsl");
		m_HdrToCubeMapShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/HDRtoCube.hlsl" : "Asset/shaders/glsl/PBR/HDRtoCube.glsl");
		m_HdrCubeMapShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/MakeHDRCubeMap.hlsl" : "Asset/shaders/glsl/PBR/MakeHDRCubeMap.glsl");
		m_IrradianceMapShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/IrradianceConvolution.hlsl" : "Asset/shaders/glsl/PBR/IrradianceConvolution.glsl");
		m_PrefilterShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PBR/prefilter.hlsl" : "Asset/shaders/glsl/PBR/prefilter.glsl");
		
		m_HdrToCubeMapShader->Bind();
		m_HdrToCubeMapShader->SetInt("equirectangularMap", 0, ShaderType::PS);

		m_HdrCubeMapShader->Bind();
		m_HdrCubeMapShader->SetInt("environmentMap", 0, ShaderType::PS);

		m_IrradianceMapShader->Bind();
		m_IrradianceMapShader->SetInt("envMap", 0, ShaderType::PS);

		m_PrefilterShader->Bind();
		m_PrefilterShader->SetInt("environmentMap", 0, ShaderType::PS);

		float quadVertices[] =
		{
			-1.0f, 1.0f,  0.0f,0.0f ,
			-1.0f,-1.0f,  0.0f,1.0f ,
			 1.0f,-1.0f,  1.0f,1.0f ,
			 1.0f, 1.0f,  1.0f,0.0f  
		};

		Ref<VertexBuffer> quadBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));

		quadBuffer->SetLayout(BufferLayout::Create({
			{ShaderDataType::Float2,"a_Position"},
			{ShaderDataType::Float2,"a_TexCoords"} }, m_BRDFLutShader
			));

		unsigned int indices[] =
		{
			1,0,3,1,3,2
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6);
		m_quad = VertexArray::Create();
		m_quad->AddVertexBuffer(quadBuffer);
		m_quad->SetIndexBuffer(indexBuffer);
		m_quad->UnBind();

		AttachmentSpecification spec = { { FramebufferUsage::Color,TextureType::TextureCube,TextureFormat::RGB16_Float,"ColorBuffer1" },
										  { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" } };
		CubeMapPass = FrameBufferEx::Create(spec);
		float bias = RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? -1.0f : 1.0f;

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

		m_IrradianceCubeMap = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 32, 32);
		m_BRDFLutTexture = Texture2D::Create(TextureFormat::RG16_Float, smpDesc, 512, 512);
		CubeMapPass->InitializeTexture("DepthBuffer", texDesc, smpDesc);

		smpDesc.MIP = Filtering::LINEAR;
		m_HdrCubeMap = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 512, 512, 8);
		m_PrefilterMap = TextureCube::Create(TextureFormat::RGB16_Float, smpDesc, 128, 128, 5);
		m_PrefilterMap->GenerateMipMap();

		RenderCommand::SetCullMode(CullMode::None);
		CubeMapPass->Bind();
		RenderCommand::SetViewport(0, 0, 512, 512);
		CubeMapPass->AttachTexture(m_BRDFLutTexture, AttachmentType::Color_0, TextureType::Texture2D, 0);
		CubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		CubeMapPass->Clear();

		// Make BRDF
		m_BRDFLutShader->Bind();
		RenderCommand::DrawIndexed(m_quad);
		m_quad->UnBind();
		m_BRDFLutShader->UnBind();

		CubeMapPass->UnBind();

	}

	void PBRPreComputePass::Execute(Ref<Scene>& scene)
	{
		if (isHdrImageChange)
		{
			MakeTextureMap();
			isHdrImageChange = false;
		}
	}
	void PBRPreComputePass::MakeTextureMap()
	{
		RenderCommand::SetCullMode(CullMode::None);
		CubeMapPass->Bind();
		CubeMapPass->DetachAll();
		CubeMapPass->GetTexture("DepthBuffer")->SetSize(512, 512);
		CubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		RenderCommand::SetViewport(0, 0, 512, 512);
		//HDR image to CubeMap
		m_HdrToCubeMapShader->Bind();
		m_HdrToCubeMapShader->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		for (int i = 0; i < 6; ++i)
		{
			int index = (int)TextureType::TextureCube_PositiveX + i;
			CubeMapPass->AttachTexture(m_HdrCubeMap, AttachmentType::Color_0, (TextureType)index, 0);
			CubeMapPass->Clear();
			m_HdrToCubeMapShader->SetMat4("u_View", captureViews[i], ShaderType::VS);
			m_HdrImage->Bind(0);
			m_HdrToCubeMapShader->UpdateBuffer();
			cube->Draw();
		}
		m_HdrToCubeMapShader->UnBind();
		m_HdrCubeMap->GenerateMipMap();

		RenderCommand::SetViewport(0, 0, 32, 32);
		CubeMapPass->GetTexture("DepthBuffer")->SetSize(32, 32);
		CubeMapPass->DetachAll();
		CubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
		CubeMapPass->AttachTexture(m_IrradianceCubeMap, AttachmentType::Color_0, TextureType::TextureCube_PositiveX, 0);

		m_IrradianceMapShader->Bind();
		m_IrradianceMapShader->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		for (int i = 0; i < 6; ++i)
		{
			int index = (int)TextureType::TextureCube_PositiveX + i;
			CubeMapPass->AttachTexture(m_IrradianceCubeMap, AttachmentType::Color_0, (TextureType)index, 0);
			CubeMapPass->Clear();

			m_IrradianceMapShader->SetMat4("u_View", captureViews[i], ShaderType::VS);
			m_IrradianceMapShader->UpdateBuffer();
			m_HdrCubeMap->Bind(0);
			cube->Draw();
		}

		m_IrradianceMapShader->UnBind();

		m_PrefilterShader->Bind();
		m_PrefilterShader->SetMat4("u_Projection", captureProjection, ShaderType::VS);
		unsigned int maxMipLevels = 5;
		for (int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);

			CubeMapPass->GetTexture("DepthBuffer")->SetSize(mipWidth, mipHeight);
			CubeMapPass->DetachAll();
			CubeMapPass->AttachTexture("DepthBuffer", AttachmentType::Depth_Stencil, TextureType::Texture2D, 0);
			RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);

			float roughtness = (float)mip / (float)(maxMipLevels - 1);
			m_PrefilterShader->SetFloat("roughness", roughtness, ShaderType::PS);
			for (int i = 0; i < 6; ++i)
			{
				int index = (int)TextureType::TextureCube_PositiveX + i;
				m_PrefilterShader->SetMat4("u_View", captureViews[i], ShaderType::VS);
				CubeMapPass->AttachTexture(m_PrefilterMap, AttachmentType::Color_0, (TextureType)index, mip);
				CubeMapPass->Clear();
				m_HdrCubeMap->Bind(0);
				m_PrefilterShader->UpdateBuffer();
				cube->Draw();
			}
		}
		m_PrefilterShader->UnBind();
		CubeMapPass->UnBind();
	}
}

