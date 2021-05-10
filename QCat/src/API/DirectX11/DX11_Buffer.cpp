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
	DX11ConstantBuffer::DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot, const void* data, unsigned int size,bool dynamic)
		:slot(slot), IsDynamic(dynamic)
	{
		QCAT_PROFILE_FUNCTION();
		this->pgfx = &gfx;

		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.Usage = IsDynamic ? D3D11_USAGE_DYNAMIC :D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = IsDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = CalculateSize(size);
		bd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = data;
		HRESULT result = gfx.GetDevice()->CreateBuffer(&bd, &sd, &pConstantBuffer);	
	}
	DX11ConstantBuffer::DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot, unsigned int size, bool dynamic)
		:slot(slot), IsDynamic(dynamic)
	{
		QCAT_PROFILE_FUNCTION();
		this->pgfx = &gfx;
		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.Usage = IsDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = IsDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = CalculateSize(size);
		bd.StructureByteStride = 0u;

		pgfx->GetDevice()->CreateBuffer(&bd, nullptr, &pConstantBuffer);
	}
	DX11ConstantBuffer::DX11ConstantBuffer(ElementLayout& layout, unsigned int slot, bool dynamic)
		:slot(slot),bufferElements(layout), IsDynamic(dynamic)
	{
		QCAT_PROFILE_FUNCTION();
		this->pgfx = QGfxDeviceDX11::GetInstance();
		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.Usage = IsDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = IsDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = (UINT)bufferElements.GetSizeInBytes();
		bd.StructureByteStride = 0u;
		
		pgfx->GetDevice()->CreateBuffer(&bd, nullptr, &pConstantBuffer);
	}
	void DX11ConstantBuffer::UpdateData()
	{
		QCAT_PROFILE_FUNCTION();

		if (!IsDynamic)
		{
			pgfx->GetContext()->UpdateSubresource(pConstantBuffer.Get(), 0, 0, bufferElements.GetData(), 0u, 0u);
		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			pgfx->GetContext()->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			memcpy(msr.pData, bufferElements.GetData(), bufferElements.GetSizeInBytes());
			pgfx->GetContext()->Unmap(pConstantBuffer.Get(), 0u);
		}
	}

	ElementRef DX11ConstantBuffer::AccessElement(const std::string& name)
	{
		size_t dot = name.find_first_of('.');
		size_t firstbrackets = name.find_first_of('[');;
		size_t secondbrackets = name.find_first_of(']');

		std::string valueName;
		std::string restofstring = name;
		size_t index = 0;
		ElementRef elementRef;
		// if there is no dot and no bracket is just variable
		if (dot == std::string::npos && firstbrackets == std::string::npos)
		{
			valueName = name;
			elementRef = bufferElements[valueName];
			restofstring = "";
			return elementRef;
		}
		// if there is no dot but bracket this is array
		else if (dot == std::string::npos && firstbrackets != std::string::npos)
		{
			std::string sindex = restofstring.substr(firstbrackets + 1, (secondbrackets - 1) - firstbrackets);
			index = std::stoi(sindex);
			valueName = restofstring.substr(0u, firstbrackets);
			elementRef = (bufferElements[valueName][index]);
			restofstring = restofstring.substr(secondbrackets + 1, restofstring.length());
			return elementRef;
		}
		// there is dot but no bracket this is struct
		else if (dot != std::string::npos && firstbrackets == std::string::npos)
		{
			valueName = restofstring.substr(0u, dot);
			restofstring = restofstring.substr(dot + 1, restofstring.length());
			elementRef = (bufferElements[valueName]);
		}
		// there is dot and bracket this is struct array
		else if (dot != std::string::npos && firstbrackets != std::string::npos)
		{
			std::string sindex = restofstring.substr(firstbrackets+1, (secondbrackets-1)-firstbrackets );
			index = std::stoi(sindex);
			// a[0].member1...
			if (dot > firstbrackets)
			{
				valueName = restofstring.substr(0u, firstbrackets);
				elementRef = (bufferElements[valueName][index]);
			}
			// a.member1[0]....
			else if (firstbrackets > dot)
			{
				valueName = restofstring.substr(0u, dot);
				elementRef = (bufferElements[valueName]);
			}
			restofstring = restofstring.substr(dot+1, restofstring.length()-dot);
		}

		if (elementRef.GetDataType() == ShaderDataType::None)
			return elementRef;
		while (true)
		{
			dot = restofstring.find_first_of('.');
			firstbrackets = restofstring.find_first_of('[');;
			secondbrackets = restofstring.find_first_of(']');

			// if there is no dot and no bracket is just variable
			if (dot == std::string::npos && firstbrackets == std::string::npos)
			{
				valueName = restofstring;
				elementRef = (elementRef[valueName]);
				restofstring = "";
				return elementRef;
			}
			// if there is no dot but bracket this is array
			else if (dot == std::string::npos && firstbrackets != std::string::npos)
			{
				std::string sindex = restofstring.substr(firstbrackets+1, (secondbrackets-1)-firstbrackets );
				index = std::stoi(sindex);
				//valueName = restofstring.substr(dot+1, (firstbrackets-1) - dot);
				valueName = restofstring.substr(0u, firstbrackets);
				elementRef = (elementRef[valueName][index]);
				restofstring = restofstring.substr(secondbrackets + 1, restofstring.length() - secondbrackets);
				return elementRef;
			}
			// there is dot but no bracket this is struct
			else if (dot != std::string::npos && firstbrackets == std::string::npos)
			{
				valueName = restofstring.substr(0u, dot);
				restofstring = restofstring.substr(dot + 1, restofstring.length());
				elementRef = (elementRef[valueName]);
			}
			// there is dot and bracket this is struct array
			else if (dot != std::string::npos && firstbrackets != std::string::npos)
			{
				std::string sindex = restofstring.substr(firstbrackets + 1, (secondbrackets - 1) - firstbrackets);
				index = std::stoi(sindex);
				// a[0].member1...
				if (dot > firstbrackets)
				{
					valueName = restofstring.substr(0u, firstbrackets);
					elementRef = (elementRef[valueName][index]);
				}
				// a.member1[0]....
				else if (firstbrackets > dot)
				{
					valueName = restofstring.substr(0u, dot);
					elementRef = (elementRef[valueName]);
				}
				restofstring = restofstring.substr(dot + 1, restofstring.length() - dot);
			}
		}
	}
}

