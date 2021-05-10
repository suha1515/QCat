#include "qcpch.h"
#include "DX11_ConstantBuffer.h"

namespace QCat
{
	DXConstantBuffer::DXConstantBuffer(uint32_t size, uint32_t slot)
		:slot(slot)
	{
		auto gfx = QGfxDeviceDX11::GetInstance();

		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = CalculateSize(size);
		bd.StructureByteStride = 0u;

		gfx->GetDevice()->CreateBuffer(&bd, nullptr, &pConstantBuffer);
	}
	DXConstantBuffer::~DXConstantBuffer()
	{
	}
	void DXConstantBuffer::Bind(uint32_t bindingpoint, Type type)
	{
		auto gfx = QGfxDeviceDX11::GetInstance();
		switch (type)
		{
		case Type::None: QCAT_ASSERT(false, "In Dx constantBuffer Type Cant be None"); break;
		case Type::Vetex: gfx->GetContext()->VSSetConstantBuffers(bindingpoint, 1u, pConstantBuffer.GetAddressOf()); break;
		case Type::Pixel: gfx->GetContext()->PSSetConstantBuffers(bindingpoint, 1u, pConstantBuffer.GetAddressOf()); break;
		case Type::Geometry:gfx->GetContext()->GSSetConstantBuffers(bindingpoint, 1u, pConstantBuffer.GetAddressOf()); break;
		}
	}
	void DXConstantBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		auto gfx = QGfxDeviceDX11::GetInstance();
		D3D11_MAPPED_SUBRESOURCE msr;
		gfx->GetContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		char* offsetpointer = (char*)msr.pData;
		memcpy(offsetpointer + offset, data, size);
		gfx->GetContext()->Unmap(pConstantBuffer.Get(), 0u);
	}
}
