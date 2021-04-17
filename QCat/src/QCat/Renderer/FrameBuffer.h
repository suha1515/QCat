#pragma once
#include "QCat/Core/QCatCore.h"
#include "Texture.h"
#include "enums.h"


namespace QCat
{
	struct FramebufferSpecification
	{
		FramebufferSpecification() = default;
		FramebufferSpecification(FramebufferUsage usage, TextureType textureType, TextureFormat dataFormat)
			: usage(usage), textureType(textureType),textureformat(dataFormat){}

	public:
		FramebufferUsage usage = FramebufferUsage::None;
		TextureType textureType = TextureType::None;
		TextureFormat textureformat = TextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification( std::initializer_list<FramebufferSpecification> attachments)
			:Attachments(attachments){}

		std::vector< FramebufferSpecification> Attachments;
	};
	struct FrameBufferSpecification
	{
		uint32_t Width=0,Height=0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		virtual ~FrameBuffer() = default;

		virtual void Bind() =0;
		virtual void UnBind()=0;

		virtual void Resize(uint32_t width,uint32_t height)=0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y, int z = 0) = 0;
		virtual void ReadPixel(uint32_t attachmentIndex, TextureFormat format,void* value, int x, int y, int z = 0) = 0;

		virtual void AttachColorBuffer(uint32_t slot,uint32_t index,int mipmap=0, int faceindex=-1) = 0;
		virtual void AttachCubeMapByIndex(uint32_t faceindex) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value) = 0;
		virtual void ClearDepthStencil() = 0;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual void* GetDepthAttachmentRendererID() const = 0;

		virtual void SaveColorBuffer(uint32_t index = 0) const = 0;

		virtual void BindColorTexture(uint32_t slot, uint32_t index = 0) const = 0;
		virtual void BindDepthTexture(uint32_t slot) const =0;

		virtual void UnBindTexture() = 0;
		virtual void Clear(glm::vec4 color = {0.1f,0.1f,0.1f,1.0f }) const = 0;

		virtual Ref<Texture> GetColorTexture(uint32_t index = 0) const = 0;
		virtual Ref<Texture> GetDepthTexture()const = 0;

		virtual void SetColorTexture(uint32_t index,Ref<Texture> texture)  = 0;
		virtual void SetDepthTexture(Ref<Texture> texture, FramebufferUsage usage)  = 0;
		virtual void SetViewport(uint32_t width, uint32_t height) = 0;

		virtual void CopyFrameBuffer(int srcx0, int srcy0, int srcx1, int srcy1, int dstx0, int dsty0, int dstx1, int dsty1, BufferBit buffer, void* destBuffer) =0;
		//virtual FrameBufferSpecification& GetSpecification() = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		static Ref <FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
} 