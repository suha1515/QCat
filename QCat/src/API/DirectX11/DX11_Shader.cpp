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
	DX11Shader::DX11Shader(const std::string& vertexShaderPath, const std::string& pixelShaderPath)
	{
		QGfxDeviceDX11* pgfx = QGfxDeviceDX11::GetInstance();
		pvs = new DX11VertexShader(*pgfx,vertexShaderPath);
		pps = new DX11PixelShader(*pgfx, pixelShaderPath);
	}
	DX11Shader::~DX11Shader()
	{
		delete pvs;
		delete pps;
	}
	void DX11Shader::Bind()
	{
		pvs->Bind();
		pps->Bind();
	}
	void DX11Shader::UnBind()
	{
		pvs->UnBind();
		pps->UnBind();
	}
	std::vector<char>& DX11Shader::GetVerexData()
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		return pvs->GetData();
	}
	DX11VertexShader::DX11VertexShader(Graphics& graphics, const std::string& path)
	{
		this->gfx = dynamic_cast<QGfxDeviceDX11*>(&graphics);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobVertex;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobVertex);
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
	DX11PixelShader::DX11PixelShader(Graphics& graphics, const std::string& path)
	{
		this->gfx = dynamic_cast<QGfxDeviceDX11*>(&graphics);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobPixel);
		char* ptr = (char*)pBlobPixel->GetBufferPointer();
		unsigned int size = pBlobPixel->GetBufferSize();

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