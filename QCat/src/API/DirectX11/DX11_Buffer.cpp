#include "qcpch.h"
#include "DX11_Buffer.h"

namespace QCat
{
	DX11VertexBuffer::DX11VertexBuffer(float* vertices,unsigned int size,unsigned int stride)
		:m_stride(stride)
	{
		//vertex Bufferf
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;

		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
		if(result == S_OK)
		{ }
	}
	void DX11VertexBuffer::Bind() const
	{
		const UINT offset = 0u;
		QGfxDeviceDX11::GetInstance()->GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
	}
	void DX11VertexBuffer::UnBind() const
	{
	}
	DX11IndexBuffer::DX11IndexBuffer(unsigned int* indices,unsigned int size)
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = size;
		ibd.StructureByteStride = sizeof(unsigned int);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&ibd, &isd, &m_pIndexBuffer);
		if (result == S_OK)
		{
		}
	}
	void DX11IndexBuffer::Bind() const
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}
	void DX11IndexBuffer::UnBind() const
	{
	}
}

