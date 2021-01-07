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

		virtual void* GetColorAttachmentRendererID() const override { return m_renderTarget->GetTexture(); }
		virtual void SaveColorBuffer() const { m_renderTarget->SaveTexture(); }
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		Ref<DX11RenderTarget> m_renderTarget;
		Ref<DX11DepthStencil> m_depthStencil;
		FrameBufferSpecification m_Specification;
	};
}