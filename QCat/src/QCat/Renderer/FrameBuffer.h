#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	enum class FramebufferUsage
	{
		None = 0,
		Color, Depth, Depth_Stencil
	};
	enum class TextureType
	{
		None=0,
		Texture2D,TextureArray,TextureCube
	};
	enum class TextureDataFormat
	{
		None = 0,
		// 32 bit
		RGBA8, RED32_INTEGER, RG16_Float, RGBA32_Float,
		// 24bit
		RGB8,
		// DepthStencil
		DEPTH24STENCIL8,
		DEPTH32
	};
	struct FramebufferSpecification
	{
		FramebufferSpecification() = default;
		FramebufferSpecification(FramebufferUsage usage, TextureType textureType, TextureDataFormat dataFormat)
			: usage(usage), textureType(textureType),textureformat(dataFormat){}

		FramebufferUsage usage = FramebufferUsage::None;
		TextureType textureType = TextureType::None;
		TextureDataFormat textureformat = TextureDataFormat::None;
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
		virtual ~FrameBuffer() = default;

		virtual void Bind() =0;
		virtual void UnBind()=0;

		virtual void Resize(uint32_t width,uint32_t height)=0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y, int z = 0) = 0;
		virtual void ReadPixel(uint32_t attachmentIndex, TextureDataFormat format,void* value, int x, int y, int z = 0) = 0;

		virtual void AttachCubeMapByIndex(uint32_t faceindex) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value) = 0;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual void* GetDepthAttachmentRendererID() const = 0;

		virtual void SaveColorBuffer(uint32_t index = 0) const = 0;

		virtual void BindColorTexture(uint32_t slot, uint32_t index = 0) const = 0;
		virtual void BindDepthTexture(uint32_t slot) const =0;

		virtual void UnBindTexture() = 0;
		virtual void Clear(glm::vec4 color = {0.1f,0.1f,0.1f,1.0f }) const = 0;
		//virtual FrameBufferSpecification& GetSpecification() = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		static Ref <FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
} 