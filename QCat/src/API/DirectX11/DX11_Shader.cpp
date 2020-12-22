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
	std::string DX11Shader::GetShaderName(const std::string& path)
	{
		// assets/shaders/hlsl/Solid_PS.hlsl
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = path.rfind('.');
		auto lastUnderbar = path.find_last_of('_');
		auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
		auto name = path.substr(lastSlash, count);
		return name;
	}
	Ref<Shader> DX11Shader::CreateShaderFromNative(const std::string& name, const std::string& src)
	{
		Ref<Shader> shader;
		auto lastUnderbar = name.find_last_of('_');
		std::string type = name.substr(lastUnderbar + 1, std::string::npos);
		if (type == "PS")
			shader = CreatePixelShaderFromNative(name, src);
		else if (type == "VS")
			shader = CreateVertexShaderFromNative(name, src);
		else
		{
			QCAT_CORE_ASSERT(false, "Undefined hlsl shader type!");
		}
		return shader;
	}
	Ref<Shader> DX11Shader::CreateShaderFromFile(const std::string& path)
	{
		// assets/shaders/hlsl/Solid_PS.hlsl
		Ref<Shader> shader;
		auto lastUnderbar = path.rfind('_');
		auto type = path.substr(lastUnderbar + 1, 2);
		if (type == "PS")
			shader = CreatePixelShaderFromFile(path);
		else if (type == "VS")
			shader = CreateVertexShaderFromFile(path);
		else
		{
			QCAT_CORE_ASSERT(false, "Undefined hlsl shader type!");
			return shader;
		}
		return shader;
	}
	Ref<DX11VertexShader> DX11Shader::CreateVertexShaderFromNative(const std::string& name, const std::string& src)
	{
		//auto lastUnderbar = name.find_last_of('_');
		//std::string type = name.substr(lastUnderbar + 1, std::string::npos);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(shaderType::VS, src);
		return std::make_shared<DX11VertexShader>(name, pBlob);
	}
	Ref<DX11PixelShader> DX11Shader::CreatePixelShaderFromNative(const std::string& name, const std::string& src)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(shaderType::PS, src);
		return std::make_shared<DX11PixelShader>(name, pBlob);
	}
	Ref<DX11VertexShader> DX11Shader::CreateVertexShaderFromFile(const std::string& path)
	{
		const std::string name = GetShaderName(path);
		return std::make_shared<DX11VertexShader>(name, path);
	}
	Ref<DX11PixelShader> DX11Shader::CreatePixelShaderFromFile(const std::string& path)
	{
		auto name = GetShaderName(path);
		return std::make_shared<DX11PixelShader>(name, path);
	}
	Microsoft::WRL::ComPtr<ID3DBlob> DX11Shader::Compile(const shaderType& type, const std::string& src)
	{
		{
			char* errormsg = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
			switch (type)
			{
			case shaderType::VS:
			{
				HRESULT hr = D3DCompile(src.c_str(), src.length(), errormsg,
					nullptr, nullptr, "main", "vs_5_0",
					D3DCOMPILE_ENABLE_STRICTNESS, 0,
					&pBlob, nullptr);
				if (FAILED(hr))
				{
					QCAT_CORE_ASSERT(false, errormsg);
				}
				return pBlob;
				break;
			}
			case shaderType::PS:
			{
				HRESULT hr = D3DCompile(src.c_str(), src.length(), errormsg,
					nullptr, nullptr, "main", "ps_5_0",
					D3DCOMPILE_ENABLE_STRICTNESS, 0,
					&pBlob, nullptr);
				if (FAILED(hr))
				{
					QCAT_CORE_ASSERT(false, errormsg);
				}
				return pBlob;
				break;
			}
			case shaderType::GS:
				break;
			}
			QCAT_CORE_ASSERT(false, "Compile Faile!");
			return pBlob;
		}
	}
	DX11Shader::DX11Shader(const std::string& name ,const Ref<Shader>& vertexShader, const Ref<Shader>& pixelShader)
		:m_name(name)
	{
		QGfxDeviceDX11* pgfx = QGfxDeviceDX11::GetInstance();
		pvs = std::dynamic_pointer_cast<DX11VertexShader>(vertexShader);
		pps = std::dynamic_pointer_cast<DX11PixelShader>(pixelShader);
		//pvs = new DX11VertexShader(vertexShaderPath, compile);
		//pps = new DX11PixelShader(pixelShaderPath, compile);
	}
	DX11Shader::DX11Shader(const std::string& name ,const std::string& vertexName,const std::string& vertexSrc, const std::string& pixelName, const std::string& pixelSrc, bool compile )
		:m_name(name)
	{
		if (compile)
		{
			pvs = CreateVertexShaderFromNative(vertexName, vertexSrc);
			pps = CreatePixelShaderFromNative(pixelName, pixelSrc);
		}
		else
		{
			pvs = CreateVertexShaderFromFile(vertexSrc);
			pps = CreatePixelShaderFromFile(pixelSrc);
		}
	}
	DX11Shader::~DX11Shader()
	{
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
		// TODO: 여기에 반환 구문을 삽입합니다.
		return pvs->GetData();
	}
	DX11VertexShader::DX11VertexShader(const std::string& name, const std::string& path)
		:m_name(name)
	{
		this->gfx = QGfxDeviceDX11::GetInstance();
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
	DX11VertexShader::DX11VertexShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
		:m_name(name)
	{
		this->gfx = QGfxDeviceDX11::GetInstance();
		char* ptr = (char*)pBlob->GetBufferPointer();
		unsigned int size = pBlob->GetBufferSize();
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
	void DX11VertexShader::Bind() const
	{
		gfx->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}
	void DX11VertexShader::UnBind() const
	{
		gfx->GetContext()->VSSetShader(nullptr, nullptr, 0u);
	}
	std::vector<char>& DX11VertexShader::GetData()
	{
		return data;
	}
	DX11PixelShader::DX11PixelShader(const std::string& name, const std::string& path)
		:m_name(name)
	{
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobPixel);
		
		HRESULT result =gfx->GetDevice()->CreatePixelShader(pBlobPixel->GetBufferPointer(),
			pBlobPixel->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::DX11PixelShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
		:m_name(name)
	{
		this->gfx = QGfxDeviceDX11::GetInstance();
		HRESULT result = gfx->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::~DX11PixelShader()
	{
	}
	void DX11PixelShader::Bind() const
	{
		gfx->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}
	void DX11PixelShader::UnBind() const
	{
		gfx->GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}
}