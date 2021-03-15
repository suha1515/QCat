#pragma once
#include "QCat/Renderer/FrameBuffer.h"
#include "API/Opengl/OpenGLTexture.h"

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
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y, int z = 0) override;
		virtual void ReadPixel(uint32_t attachmentIndex, TextureDataFormat format, void* value, int x, int y, int z = 0) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value)  override;

		virtual void AttachCubeMapByIndex(uint32_t faceindex) override;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual void* GetDepthAttachmentRendererID() const override { return reinterpret_cast<void*>(m_DepthAttachment); }

		virtual void  SaveColorBuffer(uint32_t index = 0) const override {}
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void BindColorTexture(uint32_t slot,uint32_t index = 0) const override;
		virtual void BindDepthTexture(uint32_t slot) const override;

		virtual void UnBindTexture() override {};
		virtual void Clear(glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f }) const override;
	private:
		uint32_t m_RendererID=0;
		FrameBufferSpecification m_Specification;

		std::vector<FramebufferSpecification> m_ColorAttachmentSpecifications;
		FramebufferSpecification m_DepthAttacmentSpecifications = { FramebufferUsage::None,TextureType::None,TextureDataFormat::None };

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0,m_ColorAttachment=0;
	};
}