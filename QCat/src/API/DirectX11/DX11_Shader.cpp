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
	std::string DXShader::GetShaderName(const std::string& path)
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
	//Ref<Shader> DXShader::CreateShaderFromNative(const std::string& name, const std::string& src)
	//{
	//	Ref<Shader> shader;
	//	auto lastUnderbar = name.find_last_of('_');
	//	std::string type = name.substr(lastUnderbar + 1, std::string::npos);
	//	if (type == "PS")
	//		shader = CreatePixelShaderFromNative(name, src);
	//	else if (type == "VS")
	//		shader = CreateVertexShaderFromNative(name, src);
	//	else
	//	{
	//		QCAT_CORE_ASSERT(false, "Undefined hlsl shader type!");
	//	}
	//	return shader;
	//}
	//Ref<Shader> DXShader::CreateShaderFromFile(const std::string& path)
	//{
	//	// assets/shaders/hlsl/Solid_PS.hlsl
	//	Ref<Shader> shader;
	//	auto lastUnderbar = path.rfind('_');
	//	auto type = path.substr(lastUnderbar + 1, 2);
	//	if (type == "PS")
	//		shader = CreatePixelShaderFromFile(path);
	//	else if (type == "VS")
	//		shader = CreateVertexShaderFromFile(path);
	//	else
	//	{
	//		QCAT_CORE_ASSERT(false, "Undefined hlsl shader type!");
	//		return shader;
	//	}
	//	return shader;
	//}
	Ref<DX11VertexShader> DXShader::CreateVertexShaderFromNative(const std::string& name, const std::string& src)
	{
		//auto lastUnderbar = name.find_last_of('_');
		//std::string type = name.substr(lastUnderbar + 1, std::string::npos);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(DXshaderType::VS, src);
		return std::make_shared<DX11VertexShader>(name, pBlob);
	}
	Ref<DX11PixelShader> DXShader::CreatePixelShaderFromNative(const std::string& name, const std::string& src)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(DXshaderType::PS, src);
		return std::make_shared<DX11PixelShader>(name, pBlob);
	}
	Ref<DX11VertexShader> DXShader::CreateVertexShaderFromFile(const std::string& path)
	{
		const std::string name = GetShaderName(path);
		return std::make_shared<DX11VertexShader>(name, path);
	}
	Ref<DX11PixelShader> DXShader::CreatePixelShaderFromFile(const std::string& path)
	{
		auto name = GetShaderName(path);
		return std::make_shared<DX11PixelShader>(name, path);
	}
	Microsoft::WRL::ComPtr<ID3DBlob> DXShader::Compile(const DXshaderType& type, const std::string& src)
	{
		{
			char* errormsg = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
			switch (type)
			{
			case DXshaderType::VS:
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
			case DXshaderType::PS:
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
			case DXshaderType::GS:
				break;
			}
			QCAT_CORE_ASSERT(false, "Compile Faile!");
			return pBlob;
		}
	}

	DXShader::DXShader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile)
		:m_name(name)
	{
		pvs = CreateVertexShaderFromFile(vertexFile);
		pps = CreatePixelShaderFromFile(pixelFile);
	}
	DXShader::DXShader(const std::string& name, const std::string& vertexName, const std::string& vertexSrc, const std::string& pixelName, const std::string& pixelSrc, bool compile)
		: m_name(name)
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
	DXShader::~DXShader()
	{
	}
	void DXShader::Bind() const
	{
		pvs->Bind();
		pps->Bind();
	}
	void DXShader::UnBind() const
	{
		pvs->UnBind();
		pps->UnBind();
	}
	void DXShader::SetInt(const std::string& name, int value)
	{
	}
	void DXShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UpdateConstantBuffer(name, &value);
	}
	void DXShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UpdateConstantBuffer(name, &value);
	}
	void DXShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UpdateConstantBuffer(name, &value);
	}
	void DXShader::SetFloat3u(const std::string& uniformname, const std::string& valuename, const glm::vec3& value)
	{
		UpdateConstantBuffer(uniformname, valuename, &value);
	}
	void DXShader::SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value)
	{
		UpdateConstantBuffer(uniformname, valuename, &value);
	}
	void DXShader::SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value)
	{
		UpdateConstantBuffer(uniformname, valuename, &value);
	}
	void DXShader::UpdateConstantBuffer(const std::string& uniformname, const::std::string& valuename, const void* pdata)
	{
		bool suceed = true;
		Ref<DX11ConstantBuffer>& constantbuf = pvs->GetConstantBuffer(uniformname);
		if (constantbuf)
		{
			suceed = true;
			constantbuf->UpdateElement(valuename, pdata);
		}
		else
			suceed = false;

		constantbuf = pps->GetConstantBuffer(uniformname);
		if (constantbuf)
		{
			suceed = true;
			constantbuf->UpdateElement(valuename, pdata);
		}
		else
			suceed = false;
		if (!suceed)
		{
			QCAT_CORE_ERROR("{0} cant be find from all of {1}'s {2} constantbuffer", valuename,m_name, uniformname);
		}
	}
	void DXShader::UpdateConstantBuffer(const std::string& uniformname, const void* pdata)
	{
		UpdateVertexConstantBuffer(uniformname, pdata);
		UpdatePixelConstantBuffer(uniformname, pdata);
	}
	bool DXShader::UpdateVertexConstantBuffer(const std::string& name, const void* data)
	{
		Ref<DX11ConstantBuffer>& constantbuf = pvs->GetConstantBuffer(name);
		if (constantbuf)
		{
			constantbuf->UpdateData(*QGfxDeviceDX11::GetInstance(), data);
			return true;
		}
		return false;
	}
	bool DXShader::UpdatePixelConstantBuffer(const std::string& name, const void* data)
	{
		Ref<DX11ConstantBuffer>& constantbuf = pps->GetConstantBuffer(name);
		if (constantbuf)
		{
			constantbuf->UpdateData(*QGfxDeviceDX11::GetInstance(), data);
			return true;
		}
			return false;		
	}
	std::vector<char>& DXShader::GetVerexData()
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		return pvs->GetData();
	}
	DX11VertexShader::DX11VertexShader(const std::string& name, const std::string& path)
	{
		type = DXshaderType::VS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobVertex;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobVertex);

		D3DReflect(pBlobVertex->GetBufferPointer(), pBlobVertex->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		MakeBufferElement();
		char* ptr = (char*)pBlobVertex->GetBufferPointer();
		unsigned int size = pBlobVertex->GetBufferSize();
		data.assign(size, 0);
		for (unsigned int i = 0; i < size; ++i)
		{
			data[i] = *(ptr + i);
		}
		HRESULT result = gfx->GetDevice()->CreateVertexShader(data.data(),
			data.size(),
			nullptr, &pVertexShader);

	}
	DX11VertexShader::DX11VertexShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
	{
		type = DXshaderType::VS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		MakeBufferElement();
		char* ptr = (char*)pBlob->GetBufferPointer();
		unsigned int size = pBlob->GetBufferSize();
		data.assign(size, 0);
		for (unsigned int i = 0; i < size; ++i)
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
		for (auto& buf : m_ConstantBuffers)
			buf.second->Bind();
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
	{
		type = DXshaderType::PS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobPixel);
		D3DReflect(pBlobPixel->GetBufferPointer(), pBlobPixel->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreatePixelShader(pBlobPixel->GetBufferPointer(),
			pBlobPixel->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::DX11PixelShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
	{
		type = DXshaderType::PS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::~DX11PixelShader()
	{
	}
	void DX11PixelShader::Bind() const
	{
		for (auto& buf : m_ConstantBuffers)
			buf.second->Bind();
		gfx->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}
	void DX11PixelShader::UnBind() const
	{
		gfx->GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}

	void DX11Shader::MakeBufferElement()
	{
		//======= Make Shader Element Buffer=========
		D3D11_SHADER_DESC desc;
		pReflector->GetDesc(&desc);
		UINT countConstantBuffer = desc.ConstantBuffers;
		for (UINT i = 0; i < countConstantBuffer; ++i)
		{
			std::vector<BufferElement> elements;
			std::string constantbuffername;
			D3D11_SHADER_BUFFER_DESC shaderdesc;
			// this is not com object dont worry about release
			ID3D11ShaderReflectionConstantBuffer* constantbuffer = pReflector->GetConstantBufferByIndex(i);
			constantbuffer->GetDesc(&shaderdesc);
			constantbuffername = shaderdesc.Name;
			for (UINT j = 0; j < shaderdesc.Variables; ++j)
			{
				// this is not com object dont worry about release
				ID3D11ShaderReflectionVariable* value = constantbuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC valueDesc;
				value->GetDesc(&valueDesc);
				ID3D11ShaderReflectionType* type = value->GetType();
				D3D11_SHADER_TYPE_DESC typedesc;
				type->GetDesc(&typedesc);

				unsigned int offset = valueDesc.StartOffset;
				unsigned int size = valueDesc.Size;
				const std::string& name = valueDesc.Name;
				ShaderDataType datatype = DXShaderDataConvert(typedesc);
				BufferElement element(datatype, name, offset);
				elements.push_back(element);
			}
			Ref<DX11ConstantBuffer> pConstantBuffer;
			switch (type)
			{
			case DXshaderType::VS:
				pConstantBuffer = std::make_shared<VertexConstantBuffer>(elements, i);
				break;
			case DXshaderType::PS:
				pConstantBuffer = std::make_shared<PixelConstantBuffer>(elements, i);
				break;
			case DXshaderType::GS:
				break;
			}
			AddConstantBuffer(constantbuffername, pConstantBuffer);
		}
		//===========================================
	}

	void DX11Shader::AddConstantBuffer(const std::string& name, Ref<DX11ConstantBuffer>& pConstantBuffer)
	{
		if (m_ConstantBuffers.size() > 15)
		{
			std::string error;
			if (type == DXshaderType::VS)
				error = "VertexShader";
			else if (type == DXshaderType::PS)
				error = "PixelShader";
			QCAT_CORE_ERROR("{0} exceeds constantBuffer count", error);
			QCAT_CORE_ASSERT(false, "DX11 CosntantBuffer can't be set more than 16");
			return;
		}
		m_ConstantBuffers.emplace(name, pConstantBuffer);
	}

	Ref<DX11ConstantBuffer> DX11Shader::GetConstantBuffer(const std::string name)
	{
		auto& iter = m_ConstantBuffers.find(name);
		if (iter == m_ConstantBuffers.end())
		{
			//QCAT_CORE_ERROR("Cant find vertexConstantBuffer! : Invalid name for buffer");
			return nullptr;
		}
		return iter->second;
	}

}