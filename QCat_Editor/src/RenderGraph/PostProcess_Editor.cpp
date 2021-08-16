#include "PostProcess_Editor.h"

namespace QCat
{
	PostProcessPass::PostProcessPass(unsigned int level, const std::string& name)
	{
		this->level = level;
		this->name = name;

		RegisterInput(TextureInput::Make("PBRShader_ColorBuffer", m_ColorBuffer));
		RegisterInput(TextureInput::Make("PBRShader_DepthBuffer", m_DepthBuffer));
		RegisterInput(TextureInput::Make("ScreenTexture", m_ScreenTexture));


		AttachmentSpecification spec = { { FramebufferUsage::Color,TextureType::Texture2D,TextureFormat::RGBA8,"ColorBuffer" },
										 { FramebufferUsage::Depth_Stencil,TextureType::Texture2D,TextureFormat::DEPTH24STENCIL8,"DepthBuffer" }};

		m_ScreenFrameBuffer = FrameBufferEx::Create(spec);

		m_ColorConstantBuffer = ConstantBuffer::Create(sizeof(Color), 0);
		m_OutlineConstantBuffer = ConstantBuffer::Create(sizeof(Outline), 0);
	}
	PostProcessPass::~PostProcessPass()
	{
	}
	void PostProcessPass::Initialize()
	{
		m_ScreenShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PostProcessing/ScreenShader.hlsl" : "Asset/shaders/glsl/PostProcessing/ScreenShader.glsl");
		m_OutlineShader = ShaderLibrary::Load(RenderAPI::GetAPI() == RenderAPI::API::DirectX11 ? "Asset/shaders/hlsl/PostProcessing/OutlineEffect.hlsl" : "Asset/shaders/glsl/PostProcessing/OutlineEffect.glsl");
	}
	void PostProcessPass::Execute(Ref<Scene>& scene)
	{
		Texture_Desc desc = m_ColorBuffer->GetDesc();
		RenderCommand::SetViewport(0, 0, desc.Width, desc.Height);
		

		//Outline Stencil making
		RenderCommand::SetStencilTest(true);
		RenderCommand::SetDepthTest(false);
		RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::EQUAL, 1.0);
		RenderCommand::SetFrontStencilOp(STENCIL_OP::KEEP, STENCIL_OP::KEEP, STENCIL_OP::KEEP);

		//RenderCommand::Set
		m_ScreenFrameBuffer->Bind();
		m_ScreenFrameBuffer->DetachAll();
		m_ScreenFrameBuffer->AttachTexture(m_ScreenTexture, AttachmentType::Color_0, TextureType::Texture2D);
		m_ScreenFrameBuffer->Clear();
		m_ScreenFrameBuffer->AttachTexture(m_DepthBuffer, AttachmentType::Depth_Stencil, TextureType::Texture2D);

		m_ScreenShader->Bind();
		m_ColorConstantBuffer->Bind(0, Type::Pixel);
		Color col;
		col.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		m_ColorConstantBuffer->SetData(&col, sizeof(Color), 0);
		TextureLibrary::Load("white1x1tex")->Bind(0);
		//m_ColorBuffer->Bind(0);

		//FullScreen Triangle / Stencil Making done
		RenderCommand::Draw(0, 3);
		m_ScreenShader->UnBind();

		RenderCommand::SetBlend(true);
		RenderCommand::SetBlendOp(BlendOp::BLEND_ADD, BlendOp::BLEND_ADD);
		RenderCommand::SetBlendFunc(BlendFunc::BLEND_SRC_ALPHA, BlendFunc::BLEND_INV_SRC_ALPHA, BlendFunc::BLEND_ZERO, BlendFunc::BLEND_ONE);

		// Outline Draw
		RenderCommand::SetFrontStencilFunc(COMPARISON_FUNC::NOT_EQUAL, 1.0);
		m_OutlineShader->Bind();
		m_ScreenFrameBuffer->DetachAll();
		m_ScreenFrameBuffer->AttachTexture(m_ColorBuffer, AttachmentType::Color_0, TextureType::Texture2D);
		m_ScreenFrameBuffer->AttachTexture(m_DepthBuffer, AttachmentType::Depth_Stencil, TextureType::Texture2D);

		m_OutlineConstantBuffer->Bind(0,Type::Pixel);
		Outline outline;
		outline.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		outline.outlineThickness = 2.0f;
		outline.outlineThreshHold = 0.5f;
		m_OutlineConstantBuffer->SetData(&outline, sizeof(Outline), 0);
		m_ScreenTexture->Bind(0);

		RenderCommand::Draw(0, 3);
		m_OutlineShader->UnBind();

		m_ScreenFrameBuffer->UnBind();

		RenderCommand::SetBlend(false);
		RenderCommand::SetStencilTest(false);
		RenderCommand::SetDepthTest(true);

	}
}