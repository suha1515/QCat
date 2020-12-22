#include "qcpch.h"
#include "DX11_Shader.h"

namespace QCat
{
	std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}
	DX11Shader::DX11Shader(const std::string& vertexShaderPath, const std::string& pixelShaderPath, bool compile)
	{
		QGfxDeviceDX11* pgfx = QGfxDeviceDX11::GetInstance();
		pvs = new DX11VertexShader(*pgfx, vertexShaderPath, compile);
		pps = new DX11PixelShader(*pgfx, pixelShaderPath, compile);
	}
	DX11Shader::~DX11Shader()
	{
		delete pvs;
		delete pps;
	}
	void DX11Shader::Bind() const
	{
		pvs->Bind();
		pps->Bind();

		for (auto& iter : m_vertexConstantBuffers)
			iter.second->Bind(*QGfxDeviceDX11::GetInstance());
		for (auto& iter : m_pixelConstantBuffers)
			iter.second->Bind(*QGfxDeviceDX11::GetInstance());
	}
	void DX11Shader::UnBind() const
	{
		pvs->UnBind();
		pps->UnBind();
	}
	void DX11Shader::AddVertexConstantBuffer(const std::string& name, Ref<VertexConstantBuffer>& pvertexConstantBuffer)
	{
		if (m_vertexConstantBuffers.size() > 15)
		{
			QCAT_CORE_ASSERT(false, "DX11 CosntantBuffer can't be set more than 16");
			return;
		}
		m_vertexConstantBuffers.emplace(name, pvertexConstantBuffer);
	}
	void DX11Shader::AddPixelConstantBuffer(const std::string& name, Ref<PixelConstantBuffer>& ppixelConstantBuffer)
	{
		if (m_pixelConstantBuffers.size() > 15)
		{
			QCAT_CORE_ASSERT(false, "DX11 CosntantBuffer can't be set more than 16");
			return;
		}
		m_pixelConstantBuffers.emplace(name, ppixelConstantBuffer);
	}
	void DX11Shader::UpdateVertexConstantBuffer(const std::string& name, const void* data)
	{
		auto& iter = m_vertexConstantBuffers.find(name);
		if (iter == m_vertexConstantBuffers.end())
		{
			//QCAT_CORE_ERROR("Cant find vertexConstantBuffer! : Invalid name for buffer");
			return;
		}		
		iter->second->UpdateData(*QGfxDeviceDX11::GetInstance(),data);
	}
	void DX11Shader::UpdatePixelConstantBuffer(const std::string& name, const void* data)
	{
		auto& iter = m_pixelConstantBuffers.find(name);
		if (iter == m_pixelConstantBuffers.end())
		{
			//QCAT_CORE_ERROR("Cant find pixelConstantBuffer! : Invalid name for buffer");
			return;
		}
		iter->second->UpdateData(*QGfxDeviceDX11::GetInstance(), data);
	}
	std::vector<char>& DX11Shader::GetVerexData()
	{
		// TODO: ���⿡ ��ȯ ������ �����մϴ�.
		return pvs->GetData();
	}
	DX11VertexShader::DX11VertexShader(Graphics& graphics, const std::string& path, bool compile)
	{
		this->gfx = dynamic_cast<QGfxDeviceDX11*>(&graphics);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobVertex;
		if (!compile)
		{
			D3DReadFileToBlob(ToWide(path).c_str(), &pBlobVertex);
		}
		else
		{
			char *errormsg=nullptr;
			HRESULT hr = D3DCompile(path.c_str(), path.length(),errormsg,
				nullptr, nullptr, "main", "vs_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS, 0, 
				&pBlobVertex, nullptr);
			if (FAILED(hr))
			{
				QCAT_CORE_ASSERT(false, errormsg);
			}
		}	
		char* ptr = (char*)pBlobVertex->GetBufferPointer();
		unsigned int size = pBlobVertex->GetBufferSize();
		data.assign(size, 0);
		for (int i = 0; i < size; ++i)
		{
			data[i] = *(ptr + i);
		}
		HRESULT result = gfx->GetDevice()->CreateVertexShader(data.data(),
			data.size(),
			nullptr, &pVertexShader);

	}
	DX11VertexShader::~DX11VertexShader()
	{
	}
	void DX11VertexShader::Bind()
	{
		gfx->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}
	void DX11VertexShader::UnBind()
	{
		gfx->GetContext()->VSSetShader(nullptr, nullptr, 0u);
	}
	std::vector<char>& DX11VertexShader::GetData()
	{
		return data;
	}
	DX11PixelShader::DX11PixelShader(Graphics& graphics, const std::string& path, bool compile)
	{
		this->gfx = dynamic_cast<QGfxDeviceDX11*>(&graphics);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		if (!compile)
		{
			D3DReadFileToBlob(ToWide(path).c_str(), &pBlobPixel);
		}
		else
		{
			char* errormsg = nullptr;
			HRESULT hr = D3DCompile(path.c_str(), path.length(), errormsg,
				nullptr, nullptr, "main", "ps_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS, 0,
				&pBlobPixel, nullptr);
			if (FAILED(hr))
			{
				QCAT_CORE_ASSERT(false, errormsg);
			}
		}
		HRESULT result =gfx->GetDevice()->CreatePixelShader(pBlobPixel->GetBufferPointer(),
			pBlobPixel->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::~DX11PixelShader()
	{
	}
	void DX11PixelShader::Bind()
	{
		gfx->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}
	void DX11PixelShader::UnBind()
	{
		gfx->GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}
}