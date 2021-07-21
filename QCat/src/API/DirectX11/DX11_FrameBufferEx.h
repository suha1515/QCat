#pragma once
#include "QCat/Renderer/FramebufferEx.h"
#include "DX11_Texture.h"
#include "DX11_DepthStencil.h"
#include "DX11_RenderTarget.h"
namespace QCat
{
	class DirectXFrameBufferEx : public FrameBufferEx
	{
	public:
		DirectXFrameBufferEx(const AttachmentSpecification& attachments);
		virtual ~DirectXFrameBufferEx();

		virtual void Bind() override;
		virtual void UnBind()override;
		virtual void InitializeTexture(const std::string& name, const Texture_Desc& texDesc, const Sampler_Desc& samplerDesc) override;
		virtual void AttachTexture(const std::string& name, AttachmentType attachType, TextureType type, uint32_t mipLevel)override;
		virtual void AttachTexture(const Ref<Texture>& texture, AttachmentType attachType, TextureType type, uint32_t mipLevel)override;

		virtual void AttachColorTexture(const Ref<RenderTargetView>& textureView,AttachmentType type) override {};
		virtual void AttachDepthTexture(const Ref<DepthStencilView>& textureView, AttachmentType type) override {};

		virtual void DetachTexture(AttachmentType attachType) override;
		virtual void DetachAll() override;
		virtual Ref<Texture> GetTexture(const std::string& name) override;
		virtual const AttachmentSpecification& GetSpecification() const { return m_spec; }
		virtual void Clear(const glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f }) override;
	private:
		bool colorbit = false, depthbit = false, stencilbit = false;
		std::vector<AttachmentInfo> m_AttachmentSpecifications;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
		AttachmentSpecification m_spec;

		std::vector<Ref<DX11RenderTarget>> m_ColorAttachments;
		Ref<DX11DepthStencil> m_DepthAttachment;
	};
}