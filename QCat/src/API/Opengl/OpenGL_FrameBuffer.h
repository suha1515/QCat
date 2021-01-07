#pragma once
#include "QCat/Renderer/FrameBuffer.h"

namespace QCat
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind()override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void* GetColorAttachmentRendererID() const override { return (void*)m_ColorAttachment; }
		virtual void SaveColorBuffer() const override {}
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID=0;
		uint32_t m_ColorAttachment=0, m_DepthAttachment=0;
		FrameBufferSpecification m_Specification;
	};
}