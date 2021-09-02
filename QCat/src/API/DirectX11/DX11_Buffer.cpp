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
	DX11StructureBuffer::DX11StructureBuffer(uint32_t size, uint32_t count, void* pData)
	{
		this->dataSize = size;
		this->count = count;

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = size * count;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = size;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		
		if (pData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = nullptr;
			initData.pSysMem = pData;
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&desc, &initData, pBuffer.GetAddressOf());
		}
		else
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&desc, 0, pBuffer.GetAddressOf());

		// Create Shader ResourceView
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN; //For StructureBuffer
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Buffer.FirstElement = 0;
		//srvDesc.Buffer.Flags=0;
		srvDesc.Buffer.NumElements = count;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(pBuffer.Get(), &srvDesc, pBufferView.GetAddressOf());
	}
	void DX11StructureBuffer::Bind(uint32_t slot, ShaderType type) const
	{
		switch (type)
		{
		case ShaderType::VS: QGfxDeviceDX11::GetInstance()->GetContext()->VSSetShaderResources(slot, 1, pBufferView.GetAddressOf()); break;
		case ShaderType::PS: QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1, pBufferView.GetAddressOf()); break;
		case ShaderType::CS: QGfxDeviceDX11::GetInstance()->GetContext()->CSSetShaderResources(slot, 1, pBufferView.GetAddressOf()); break;
		case ShaderType::GS: QGfxDeviceDX11::GetInstance()->GetContext()->GSSetShaderResources(slot, 1, pBufferView.GetAddressOf()); break;
		default:
			QCAT_CORE_ERROR("StructBuffer Bind Error!: Shader Type Wrong!");
		}
		
	}
	void DX11StructureBuffer::ReadData(std::vector<char>& data)
	{
		D3D11_BUFFER_DESC outputDesc;
		outputDesc.Usage = D3D11_USAGE_STAGING;
		outputDesc.BindFlags = 0;
		outputDesc.ByteWidth = this->dataSize * this->count;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		outputDesc.StructureByteStride = this->dataSize;
		outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		ID3D11Buffer* outputBuffer = nullptr;
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&outputDesc, 0, &outputBuffer);
		QGfxDeviceDX11::GetInstance()->GetContext()->CopyResource(outputBuffer, pBuffer.Get());

		int size = this->dataSize * this->count;
		if (data.size() != size)
			data.resize(size);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		QGfxDeviceDX11::GetInstance()->GetContext()->Map(outputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);

		char* pData = reinterpret_cast<char*>(mappedData.pData);
		for (int i = 0; i < size; ++i)
			data[i] = (pData)[i];
		QGfxDeviceDX11::GetInstance()->GetContext()->Unmap(outputBuffer, 0);
	}
	DX11RWStructureBuffer::DX11RWStructureBuffer(uint32_t size, uint32_t count, void* pData)
	{
		this->dataSize = size;
		this->count = count;

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = size * count;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = size;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		
		if (pData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = nullptr;
			initData.pSysMem = pData;
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&desc, &initData, pBuffer.GetAddressOf());
		}
		else
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&desc, 0, pBuffer.GetAddressOf());
		//Create Unordered Access View
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.NumElements = count;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateUnorderedAccessView(pBuffer.Get(), &uavDesc, pBufferView.GetAddressOf());
	}
	void DX11RWStructureBuffer::Bind(uint32_t slot, ShaderType type) const
	{
		switch (type)
		{
		case ShaderType::CS: QGfxDeviceDX11::GetInstance()->GetContext()->CSSetUnorderedAccessViews(slot, 1, pBufferView.GetAddressOf(),nullptr); break;
		default:
			QCAT_CORE_ERROR("RWStructBuffer Bind Error!: Shader Type isnt ComputShader!");
		}
	}
	void DX11RWStructureBuffer::ReadData(std::vector<char>& data)
	{
		D3D11_BUFFER_DESC outputDesc;
		outputDesc.Usage = D3D11_USAGE_STAGING;
		outputDesc.BindFlags = 0;
		outputDesc.ByteWidth = this->dataSize * this->count;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		outputDesc.StructureByteStride = this->dataSize;
		outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		ID3D11Buffer* outputBuffer = nullptr;
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateBuffer(&outputDesc, 0, &outputBuffer);
		QGfxDeviceDX11::GetInstance()->GetContext()->CopyResource(outputBuffer, pBuffer.Get());

		int size = this->dataSize * this->count;
		if (data.size() != size)
			data.resize(size);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		QGfxDeviceDX11::GetInstance()->GetContext()->Map(outputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);

		char* pData = reinterpret_cast<char*>(mappedData.pData);
		for (int i = 0; i < size; ++i)
			data[i] = pData[i];
		QGfxDeviceDX11::GetInstance()->GetContext()->Unmap(outputBuffer, 0);
	}
}

