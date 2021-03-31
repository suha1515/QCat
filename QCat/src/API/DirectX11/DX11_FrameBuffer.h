#pragma once
#include "QGfxDeviceDX11.h"
#include "QCat/Renderer/FrameBuffer.h"
#include "DX11_DepthStencil.h"
#include "DX11_RenderTarget.h"
#include "DX11_Texture.h"
#include "DX11_Sampler.h"

namespace QCat
{
	struct RenderTargets
	{
		std::vector<Ref<DX11RenderTarget>> rendertargets;
		std::vector<Ref<Texture>> textures;
		int attachTarget = 0;
	};
	class DX11FrameBuffer : public FrameBuffer
	{
	public:
		DX11FrameBuffer(const FrameBufferSpecification& spec);
		virtual ~DX11FrameBuffer();
		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind()override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y, int z = 0) override;
		virtual void ReadPixel(uint32_t attachmentIndex, TextureFormat format, void* value, int x, int y, int z = 0)override {};

		virtual void AttachCubeMapByIndex(uint32_t faceindex)override;
		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value)  override;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const override { QCAT_CORE_ASSERT(index<m_ColorAttachments.size());return m_ColorAttachments[index].textures[0]->GetTexture(); }
		virtual void* GetDepthAttachmentRendererID() const override { return nullptr; };

		virtual void SaveColorBuffer(uint32_t index = 0) const override;
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void BindColorTexture(uint32_t slot, uint32_t index = 0) const override;
		virtual void BindDepthTexture(uint32_t slot) const override;

		virtual void UnBindTexture() override;
		virtual void Clear(glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f }) const override;
		virtual void CopyFrameBuffer(int srcx0, int srcy0, int srcx1, int srcy1, int dstx0, int dsty0, int dstx1, int dsty1, BufferBit buffer, void* destBuffer) override;

	private:
		FrameBufferSpecification m_Specification;

		std::vector<FramebufferSpecification> m_ColorAttachmentSpecifications;
		FramebufferSpecification m_DepthAttacmentSpecifications = { FramebufferUsage::None,TextureType::None,TextureFormat::None };

		std::vector<RenderTargets> m_ColorAttachments;
		Ref<DX11DepthStencil> m_DepthAttachment = 0, m_ColorAttachment = 0;
	};
}