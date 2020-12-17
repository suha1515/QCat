#pragma once
#include "Renderer/Buffer.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class QCAT_API DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(float* vertices, unsigned int size,unsigned int stride);
		~DX11VertexBuffer(){}

		virtual void Bind() const override ;
		virtual void UnBind() const override ;

		virtual const std::shared_ptr<BufferLayout>& GetLayout() const override { return m_pbufferLayout; };
		virtual void SetLayout(BufferLayout* layout) override { m_pbufferLayout.reset(layout); };
	private:
		std::string m_tag;
		UINT m_stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		std::shared_ptr<BufferLayout> m_pbufferLayout;
	};
	class QCAT_API DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(unsigned int* indices ,unsigned int size);
		~DX11IndexBuffer() {}

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual unsigned int GetCount() const override { return m_count; }
	private:
		std::string m_tag;
		UINT m_count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	};

	class QCAT_API DX11ConstantBuffer
	{
	public:
		DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot ,const void* data,unsigned int size)
			:slot(slot)
		{
			unsigned int realSize = 0;
			if (size % 16 == 0)
				realSize = size;
			else
			{
				int gap = size & 16;
				gap = 16*(1+gap)-size;
				realSize = size+gap;
			}
			
			D3D11_BUFFER_DESC bd;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = realSize;
			bd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;
			HRESULT result = gfx.GetDevice()->CreateBuffer(&bd, &sd, &pConstantBuffer);
			int a = 0;
		}
		void UpdateData(QGfxDeviceDX11& gfx,const void* data)
		{
			gfx.GetContext()->UpdateSubresource(pConstantBuffer.Get(), 0, 0, data, 0u, 0u);
		}
		~DX11ConstantBuffer() {}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		unsigned int slot;
	};
	class QCAT_API VertexConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		void Bind(QGfxDeviceDX11& gfx) 
		{
			gfx.GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
	class QCAT_API PixelConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		void Bind(QGfxDeviceDX11& gfx)
		{
			gfx.GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
}