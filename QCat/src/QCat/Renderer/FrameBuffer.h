#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	struct FrameBufferSpecification
	{
		uint32_t Width=0,Height=0;
		//FrameBufferFormat Format = ;
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

		virtual void* GetColorAttachmentRendererID() const = 0;
		virtual void SaveColorBuffer() const = 0;
		//virtual FrameBufferSpecification& GetSpecification() = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		static Ref <FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
} 