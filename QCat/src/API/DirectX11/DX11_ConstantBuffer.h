#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/ConstantBuffer.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DXConstantBuffer : public ConstantBuffer
	{
	public:
		unsigned int CalculateSize(unsigned int size)
		{
			unsigned int realSize = 0;
			if (size % 16 == 0)
				realSize = size;
			else
			{
				realSize = size + (16u - size % 16u) % 16u;;
			}
			return realSize;
		}
		DXConstantBuffer(uint32_t size,uint32_t slot);
		virtual ~DXConstantBuffer();

		virtual void Bind(uint32_t bindingpoint, Type type = Type::None) override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		unsigned int slot;
		std::string name;
	};
}