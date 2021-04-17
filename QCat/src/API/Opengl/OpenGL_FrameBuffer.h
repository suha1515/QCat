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
		virtual void ReadPixel(uint32_t attachmentIndex, TextureFormat format, void* value, int x, int y, int z = 0) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value)  override;
		virtual void ClearDepthStencil() override;

		virtual void AttachColorBuffer(uint32_t slot, uint32_t index, int mipmap = 0, int faceindex=-1) override;
		virtual void AttachCubeMapByIndex(uint32_t faceindex) override;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual void* GetDepthAttachmentRendererID() const override { return m_DepthAttachment->GetTexture(); }

		virtual void  SaveColorBuffer(uint32_t index = 0) const override {}
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void BindColorTexture(uint32_t slot,uint32_t index = 0) const override;
		virtual void BindDepthTexture(uint32_t slot) const override;

		virtual void UnBindTexture() override {};
		virtual void Clear(glm::vec4 color = { 0.1f,0.1f,0.1f,1.0f }) const override;

		virtual Ref<Texture> GetColorTexture(uint32_t index = 0) const override;
		virtual Ref<Texture> GetDepthTexture()const override;

		virtual void SetColorTexture(uint32_t index, Ref<Texture> texture)  override;
		virtual void SetDepthTexture(Ref<Texture> texture, FramebufferUsage usage)  override;
		virtual void SetViewport(uint32_t width, uint32_t height) override;

		virtual void CopyFrameBuffer(int srcx0, int srcy0, int srcx1, int srcy1, int dstx0, int dsty0, int dstx1, int dsty1, BufferBit buffer,void* destBuffer) override;
	private:
		uint32_t m_RendererID=0;
		FrameBufferSpecification m_Specification;

		std::vector<FramebufferSpecification> m_ColorAttachmentSpecifications;
		FramebufferSpecification m_DepthAttacmentSpecifications = { FramebufferUsage::None,TextureType::None,TextureFormat::None };

		std::vector<Ref<Texture>> m_ColorAttachments;
		Ref<Texture> m_DepthAttachment = 0,m_ColorAttachment=0;
	};
}