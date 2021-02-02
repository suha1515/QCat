#pragma once
#include "QGfxDeviceDX11.h"
#include "QCat/Renderer/FrameBuffer.h"
#include "DX11_DepthStencil.h"
#include "DX11_RenderTarget.h"

namespace QCat
{
	class DX11FrameBuffer : public FrameBuffer
	{
	public:
		DX11FrameBuffer(const FrameBufferSpecification& spec);
		virtual ~DX11FrameBuffer();
		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind()override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const override { QCAT_CORE_ASSERT(index<m_ColorAttachments.size());return reinterpret_cast<void*>(m_ColorAttachments[index]->GetTexture()); }
		virtual void SaveColorBuffer(uint32_t index=0) const override { m_ColorAttachments[index]->SaveTexture(); }
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		FrameBufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttacmentSpecifications = FramebufferTextureFormat::None;

		std::vector<Ref<DX11RenderTarget>> m_ColorAttachments;
		Ref<DX11DepthStencil> m_DepthAttachment = 0, m_ColorAttachment = 0;
	};
}