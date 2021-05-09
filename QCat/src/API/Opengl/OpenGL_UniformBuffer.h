#pragma once

#include "QCat/Renderer/ConstantBuffer.h"

namespace QCat
{
	class OpenGLUniformBuffer : public ConstantBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();

		virtual void Bind(uint32_t bindingpoint) override;
		virtual void SetData(const void* pdata, uint32_t size, uint32_t offset = 0) override;
	private:
		uint32_t m_renderID = 0;
	};
}