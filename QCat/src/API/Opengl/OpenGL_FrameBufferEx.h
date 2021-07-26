#pragma once
#include "QCat/Renderer/FramebufferEx.h"
#include "OpenGLTexture.h"

namespace QCat
{
	class OpenGLFrameBufferEx : public FrameBufferEx
	{
	public:
		OpenGLFrameBufferEx(const AttachmentSpecification& attachments);
		virtual ~OpenGLFrameBufferEx();

		virtual void Bind() override;
		virtual void UnBind()override;
		virtual void InitializeTexture(const std::string& name, const Texture_Desc& texDesc, const Sampler_Desc& samplerDesc) override;
		virtual void AttachTexture(const std::string& name, AttachmentType attachType, TextureType type, uint32_t mipLevel = 0, uint32_t layerStart = 0, uint32_t layerLevel = 1)override;
		virtual void AttachTexture(const Ref<Texture>& texture, AttachmentType attachType, TextureType type, uint32_t mipLevel = 0, uint32_t layerStart = 0, uint32_t layerLevel = 1)override;
		
		virtual void AttachColorTexture(const Ref<RenderTargetView>& textureView, AttachmentType type) override;
		virtual void AttachDepthTexture(const Ref<DepthStencilView>& textureView, AttachmentType type) override;

		virtual void DetachTexture(AttachmentType attachType) override;
		virtual void DetachAll() override;

		virtual const AttachmentSpecification& GetSpecification() const {return m_spec;}
		virtual Ref<Texture> GetTexture(const std::string& name) override;

		virtual void Clear(const glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f }) override;
	public:
	private:
		bool colorbit=false, depthbit=false, stencilbit=false;
		uint32_t m_RendererID;
		AttachmentSpecification m_spec;
		std::vector<AttachmentInfo> m_AttachmentSpecifications;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;

		std::vector<Ref<OpenGLRenderTargetView>> m_RenderTargets;
		Ref<OpenGLDepthStencilView> m_DepthStencilView;
	};
}
