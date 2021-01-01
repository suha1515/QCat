#include "qcpch.h"
#include "DX11_Buffer.h"

namespace QCat
{
	DX11VertexBuffer::DX11VertexBuffer(unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		//vertex Bufferf
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = 0u;

		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);
	}
	DX11VertexBuffer::DX11VertexBuffer(float* vertices,unsigned int size,unsigned int stride)
		:m_stride(stride)
	{
		QCAT_PROFILE_FUNCTION();

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
		QCAT_PROFILE_FUNCTION();

		const UINT offset = 0u;
		const UINT stride = m_pbufferLayout->GetStride();
		QGfxDeviceDX11::GetInstance()->GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
		QCAT_CORE_ASSERT(m_pbufferLayout != nullptr, "InputLayout is nullptr! : VertexBuffer Binding Error");
		m_pbufferLayout->Bind();

	}
	void DX11VertexBuffer::UnBind() const
	{
	}
	void DX11VertexBuffer::SetData(void* data, unsigned int size)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		// Map is enable cpu access the data from gpu side ( i.e buffer ,texture) and cpu can access data through the SubResource
		QGfxDeviceDX11::GetInstance()->GetContext()->Map(m_pVertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		memcpy(msr.pData, data, size);
		// UnMap Invalidate the pointer to resource(in here ID3D11Buffer) and reneable the gpu'access to that resource
		// so when Map to Resource cpu can access that resource but gpu cant access that
		// after Unmap gpu can access (cpu cant)
		QGfxDeviceDX11::GetInstance()->GetContext()->Unmap(m_pVertexBuffer.Get(), 0u);
	}
	DX11IndexBuffer::DX11IndexBuffer(unsigned int* indices,unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		m_count = size;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = m_count * sizeof(unsigned int);
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
		QCAT_PROFILE_FUNCTION();

		QGfxDeviceDX11::GetInstance()->GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}
	void DX11IndexBuffer::UnBind() const
	{
	}
}

