#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		// Color
		RGBA8 ,
		RED_INTEGER,
		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format){}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;

		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification( std::initializer_list<FramebufferTextureSpecification> attachments)
			:Attachments(attachments){}

		std::vector< FramebufferTextureSpecification> Attachments;
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
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual void* GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual void SaveColorBuffer(uint32_t index = 0) const = 0;

		virtual void BindColorTexture(uint32_t slot, uint32_t index = 0) const = 0;
		virtual void UnBindTexture() = 0;
		//virtual FrameBufferSpecification& GetSpecification() = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		static Ref <FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
} 