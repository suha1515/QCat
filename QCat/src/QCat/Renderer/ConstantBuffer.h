#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		virtual ~ConstantBuffer() {};

		virtual void Bind(uint32_t bindingpoint) = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<ConstantBuffer> Create(uint32_t size, uint32_t binding);
	};
}