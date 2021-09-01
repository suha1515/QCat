#include "qcpch.h"
#include "DX11_Shader.h"
#include "DX11_BufferLayout.h"

namespace QCat
{
	const DXShader* DXShader::s_CurrentlyBound = nullptr;
	static DXshaderType ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return DXshaderType::VS;
		if (type == "fragment" || type == "pixel")
			return DXshaderType::PS;
		if (type == "geometry")
			return DXshaderType::GS;
		if(type == "compute")
			return DXshaderType::CS;

		QCAT_CORE_ASSERT(false, "Unknown shader Type!");
		return DXshaderType::None;
	}
	std::string ReadFile(const std::string& filepath)
	{
		QCAT_PROFILE_FUNCTION();

		std::string result;
		// ios::in means read mode, ios::binary means it will read data as binary
		std::ifstream in(filepath, std::ios::in | std::ios::binary);// ifstream closes itself due to RAII
		if (in)
		{
			// seekg is move the cursor ios::end means end of file 
			// so based on end of file this func will move the cursor as first parameter value
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			// after reading you have to call close function
		}
		else
		{
			QCAT_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		return result;
	}
	
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
	Ref<DX11Shader> DXShader::CreateShaderFromNative(const std::string& name, const std::string& src,DXshaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		//auto lastUnderbar = name.find_last_of('_');
		//std::string type = name.substr(lastUnderbar + 1, std::string::npos);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(type, src);
		switch (type)
		{
		case DXshaderType::VS: return std::make_shared<DX11VertexShader>(name, pBlob);
		case DXshaderType::PS: return std::make_shared<DX11PixelShader>(name, pBlob);
		case DXshaderType::GS: return std::make_shared<DX11GeometryShader>(name, pBlob);
		}
		
	}
	Ref<DX11Shader> DXShader::CreateShaderFromFile(const std::string& path,DXshaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		auto name = GetShaderName(path);
		switch (type)
		{
		case DXshaderType::VS: return std::make_shared<DX11VertexShader>(name, path);
		case DXshaderType::PS: return std::make_shared<DX11PixelShader>(name, path);
		case DXshaderType::GS: return std::make_shared<DX11GeometryShader>(name, path);
		}
	}
	Microsoft::WRL::ComPtr<ID3DBlob> DXShader::Compile(const DXshaderType& type, const std::string& src)
	{
		QCAT_PROFILE_FUNCTION();
			char* errormsg = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
			UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef  QCAT_DEBUG
			flag |= D3DCOMPILE_DEBUG;
#endif //  QCAT_DEBUG
			switch (type)
			{
			case DXshaderType::VS:
			{
				HRESULT hr = D3DCompile(src.c_str(), src.length(), errormsg,
					nullptr, nullptr, "main", "vs_5_0",
					flag, 0,
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
					flag, 0,
					&pBlob, nullptr);
				if (FAILED(hr))
				{
					QCAT_CORE_ASSERT(false, errormsg);
				}
				return pBlob;
				break;
			}
			case DXshaderType::GS:
			{
				HRESULT hr = D3DCompile(src.c_str(), src.length(), errormsg,
					nullptr, nullptr, "main", "gs_5_0",
					flag, 0,
					&pBlob, nullptr);
				if (FAILED(hr))
				{
					QCAT_CORE_ASSERT(false, errormsg);
				}
				return pBlob;
				break;
			}
			case DXshaderType::CS:
			{
				HRESULT hr = D3DCompile(src.c_str(), src.length(), errormsg,
					nullptr, nullptr, "main", "cs_5_0",
					flag, 0,
					&pBlob, nullptr);
				if (FAILED(hr))
				{
					QCAT_CORE_ASSERT(false, errormsg);
				}
				return pBlob;
				break;
			}
			}
			QCAT_CORE_ASSERT(false, "Compile Faile!");
			return pBlob;
	}
	std::unordered_map<DXshaderType, std::string> DXShader::PreProcess(const std::string& source)
	{
		QCAT_PROFILE_FUNCTION();

		std::unordered_map<DXshaderType, std::string> shaderSources;

		const char* typeToken = "#type";
		const char* modelToken = "#model";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t endofline = source.find_first_of("\r\n", pos);
			QCAT_CORE_ASSERT(endofline != std::string::npos, "Syntax error");

			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, endofline - begin);
			QCAT_CORE_ASSERT((int)ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", endofline);
			pos = source.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeFromString(type)] = 
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
	
		}
		return shaderSources;
	}
	Microsoft::WRL::ComPtr<ID3DBlob> DXShader::Compile(const std::string& src, const std::string& profile, const std::string& main, ErrorInfo& info)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef  QCAT_DEBUG
		flag |= D3DCOMPILE_DEBUG;
#endif //  QCAT_DEBUG

		HRESULT status = D3DCompile(src.c_str(), src.size(), NULL, NULL, NULL, main.c_str(), profile.c_str(), flag, 0, &shaderBlob, &errorBlob);
		if (status != S_OK)
			info.meesage = "Reading shader code failed";
		if (errorBlob)
		{
			info.profile += profile + "\n";
			if (errorBlob->GetBufferSize())
			{
				std::string msg = (const char*)errorBlob->GetBufferPointer();
				info.meesage = profile + "Shader Compile Error!\n";
				info.meesage += msg;
				QCAT_CORE_ERROR(info.meesage);
				QCAT_CORE_ASSERT(msg.find("warning", 0) != std::string::npos,"Shader Error!")
			}
		}
		if (status == S_OK)
			return shaderBlob;
		return nullptr;
	}
	DXShader::DXShader(const std::string& shaderPath)
	{
		QCAT_PROFILE_FUNCTION();

		auto begin = shaderPath.find_last_of('.');
		std::string extension = shaderPath.substr(begin + 1, shaderPath.length());
		std::string source;
		if (extension == "hlsl")
		{
			source = ReadFile(shaderPath);
			auto shaderSources = PreProcess(source);
			
			std::string sourcecode = shaderSources[DXshaderType::VS];
			ErrorInfo info;
			Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
			if (sourcecode != "")
			{
				shaderBlob = Compile(sourcecode, "vs_5_0", "VSMain", info);
				m_Data.pvs = CreateRef<DX11VertexShader>("", shaderBlob);
			}
			sourcecode = shaderSources[DXshaderType::GS];
			if (sourcecode != "")
			{
				shaderBlob = Compile(sourcecode, "gs_5_0", "GSMain", info);
				m_Data.pgs = CreateRef<DX11GeometryShader>("", shaderBlob);
			}
			sourcecode = shaderSources[DXshaderType::PS];
			if (sourcecode != "")
			{
				shaderBlob = Compile(sourcecode, "ps_5_0", "PSMain", info);
				m_Data.pps = CreateRef<DX11PixelShader>("", shaderBlob);
			}
			sourcecode = shaderSources[DXshaderType::CS];
			if (sourcecode != "")
			{
				shaderBlob = Compile(sourcecode, "cs_5_0", "CSMain", info);
				m_Data.pcs = CreateRef<DX11ComputeShader>("", shaderBlob);
			}
		}
		else 
		{
			QCAT_CORE_ASSERT(false, "Wrong format for DXShader");
		}
		auto lastSlash = shaderPath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = shaderPath.rfind('.');
		auto count = lastDot == std::string::npos ? shaderPath.size() - lastSlash : lastDot - lastSlash;
		m_name = shaderPath.substr(lastSlash, count);
	}
	DXShader::DXShader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile, const std::string& geoFile)
		:m_name(name)
	{
		QCAT_PROFILE_FUNCTION();

		auto begin = vertexFile.find_last_of('.');

		
		if (vertexFile != "")
		{
			std::string extension = vertexFile.substr(begin + 1, vertexFile.length());
			DXshaderType type = DXshaderType::VS;
			if (extension == "hlsl")
			{
				std::string vertexName = GetShaderName(vertexFile);
				std::string vertexSrc = ReadFile(vertexFile);
				m_Data.pvs = std::dynamic_pointer_cast<DX11VertexShader>(CreateShaderFromNative(vertexName, vertexSrc, type));
			}
			else if (extension == "cso")
				m_Data.pvs = std::dynamic_pointer_cast<DX11VertexShader>(CreateShaderFromFile(vertexFile, type));
			else
				QCAT_CORE_ASSERT(false, "Wrong extension! Vertex Shader Compile Error");
		}
		else
		{
			m_Data.pvs = nullptr;
		}
		begin = pixelFile.find_last_of('.');
		if (pixelFile != "")
		{
			std::string extension = pixelFile.substr(begin + 1, pixelFile.length());
			DXshaderType type = DXshaderType::PS;
			if (extension == "hlsl")
			{
				std::string pixelSrc = ReadFile(pixelFile);
				std::string pixelName = GetShaderName(pixelFile);
				m_Data.pps = std::dynamic_pointer_cast<DX11PixelShader>(CreateShaderFromNative(pixelName, pixelSrc, type));
			}
			else if (extension == "cso")
				m_Data.pps = std::dynamic_pointer_cast<DX11PixelShader>(CreateShaderFromFile(pixelFile, type));
			else
				QCAT_CORE_ASSERT(false, "Wrong extension! Pixel Shader Compile Error");
		}
		else
		{
			m_Data.pps = nullptr;
		}
		begin = geoFile.find_last_of('.');
		if (geoFile != "")
		{
			std::string extension = geoFile.substr(begin + 1, geoFile.length());
			DXshaderType type = DXshaderType::GS;
			if (extension == "hlsl")
			{
				std::string pixelSrc = ReadFile(geoFile);
				std::string pixelName = GetShaderName(geoFile);
				m_Data.pgs = std::dynamic_pointer_cast<DX11GeometryShader>(CreateShaderFromNative(pixelName, pixelSrc, type));
			}
			else if (extension == "cso")
				m_Data.pgs = std::dynamic_pointer_cast<DX11GeometryShader>(CreateShaderFromFile(geoFile, type));
			else
				QCAT_CORE_ASSERT(false, "Wrong extension! geometry Shader Compile Error");
		}
		else
		{
			m_Data.pgs = nullptr;
		}


		
	}

	DXShader::~DXShader()
	{
	}
	void DXShader::Bind() const
	{
		QCAT_PROFILE_FUNCTION();
		s_CurrentlyBound = this;

		if(m_Data.pvs)
			m_Data.pvs->Bind();
		if(m_Data.pgs)
			m_Data.pgs->Bind();
		if(m_Data.pps)
			m_Data.pps->Bind();
		if (m_Data.pcs)
			m_Data.pcs->Bind();
	}
	void DXShader::UpdateBuffer() const
	{
		if(m_Data.pvs)
			m_Data.pvs->UpdateBuffer();
		if (m_Data.pgs)
			m_Data.pgs->UpdateBuffer();
		if(m_Data.pps)
			m_Data.pps->UpdateBuffer();
	}
	void DXShader::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		if(m_Data.pvs)
			m_Data.pvs->UnBind();
		if(m_Data.pgs)
			m_Data.pgs->UnBind();
		if(m_Data.pps)
			m_Data.pps->UnBind();
		if (m_Data.pcs)
			m_Data.pcs->UnBind();
	}
	void DXShader::SetInt(const std::string& name, int value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name,type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetIntArray(const std::string& name, int* values, unsigned int count, ShaderType type)
	{
	}
	void DXShader::SetFloat(const std::string& name, const float& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name,type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloatArray(const std::string name, float* values, unsigned int count, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		for (int i = 0; i < count; ++i)
		{
			ref.second[i] = values[i];
		}
	}
	void DXShader::SetFloat2(const std::string& name, const glm::vec2& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloat3(const std::string& name, const glm::vec3& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloat4(const std::string& name, const glm::vec4& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetMat4(const std::string& name, const glm::mat4& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetBool(const std::string& name, const bool& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name, type);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloat3u(const std::string& uniformname, const std::string& valuename, const glm::vec3& value)
	{
		/*QCAT_PROFILE_FUNCTION();

		UpdateConstantBuffer(uniformname, valuename, &value);*/
	}
	void DXShader::SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value)
	{
		/*QCAT_PROFILE_FUNCTION();

		UpdateConstantBuffer(uniformname, valuename, &value);*/
	}
	void DXShader::SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value)
	{
		/*QCAT_PROFILE_FUNCTION();

		UpdateConstantBuffer(uniformname, valuename, &value);*/
	}
	std::pair<Ref<DX11ConstantBuffer>, ElementRef> DXShader::FindVariable(const std::string& name, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();
		std::unordered_map<std::string, Ref<DX11ConstantBuffer>>* constantBuffers = nullptr;
		switch (type)
		{
		case ShaderType::VS:
			if (m_Data.pvs)
			constantBuffers = &m_Data.pvs->GetConstantBuffers();
			break;
		case ShaderType::PS:
			if (m_Data.pps)
			constantBuffers = &m_Data.pps->GetConstantBuffers();
			break;
		case ShaderType::GS:
			if(m_Data.pgs)
				constantBuffers = &m_Data.pgs->GetConstantBuffers();
			break;
		case ShaderType::CS:

			break;
		}
		if (constantBuffers !=nullptr)
		{
			std::pair<Ref<DX11ConstantBuffer>, ElementRef> pair;
			for (auto& iter : *constantBuffers)
			{
				auto ref = iter.second->FindVariable(name);
				if (ref.Exists())
					return { iter.second,ref };
			}
		}
		
		return{ nullptr, ElementRef{} };
	}
	bool DXShader::UpdateVertexConstantBuffer(const std::string& name, const void* data)
	{
		//QCAT_PROFILE_FUNCTION();

		//Ref<DX11ConstantBuffer>& constantbuf = pvs->GetConstantBuffer(name);
		//if (constantbuf)
		//{
		//	constantbuf->UpdateData(*QGfxDeviceDX11::GetInstance(), data);
		//	return true;
		//}
		//return false;
		return false;
	}
	bool DXShader::UpdatePixelConstantBuffer(const std::string& name, const void* data)
	{
		/*QCAT_PROFILE_FUNCTION();

		Ref<DX11ConstantBuffer>& constantbuf = pps->GetConstantBuffer(name);
		if (constantbuf)
		{
			constantbuf->UpdateData(*QGfxDeviceDX11::GetInstance(), data);
			return true;
		}
			return false;	*/	
		return false;

	}

	DX11VertexShader::DX11VertexShader(const std::string& name, const std::string& path)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::VS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobVertex;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobVertex);

		D3DReflect(pBlobVertex->GetBufferPointer(), pBlobVertex->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
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
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::VS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
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
		gfx->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}
	void DX11VertexShader::UpdateBuffer() const
	{
		for (auto& buf : m_ConstantBuffers)
			buf.second->FinalizeUpdate();
		for (auto& buf : m_ConstantBuffers)
			buf.second->Bind();
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
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::PS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobPixel);
		D3DReflect(pBlobPixel->GetBufferPointer(), pBlobPixel->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreatePixelShader(pBlobPixel->GetBufferPointer(),
			pBlobPixel->GetBufferSize(),
			nullptr, &pPixelShader);
	}
	DX11PixelShader::DX11PixelShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::PS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
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
	void DX11PixelShader::UpdateBuffer() const
	{
		for (auto& buf : m_ConstantBuffers)
			buf.second->FinalizeUpdate();
		for (auto& buf : m_ConstantBuffers)
			buf.second->Bind();
	}
	void DX11PixelShader::UnBind() const
	{
		gfx->GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}
	DX11GeometryShader::DX11GeometryShader(const std::string& name, const std::string& path)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::GS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobGeo;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobGeo);
		D3DReflect(pBlobGeo->GetBufferPointer(), pBlobGeo->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreateGeometryShader(pBlobGeo->GetBufferPointer(),
			pBlobGeo->GetBufferSize(),
			nullptr, &pGeometryShader);
	}

	DX11GeometryShader::DX11GeometryShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::GS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreateGeometryShader(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr, &pGeometryShader);
	}

	DX11GeometryShader::~DX11GeometryShader()
	{
	}

	void DX11GeometryShader::Bind() const
	{
		gfx->GetContext()->GSSetShader(pGeometryShader.Get(), nullptr, 0u);
	}

	void DX11GeometryShader::UpdateBuffer() const
	{
		for (auto& buf : m_ConstantBuffers)
			buf.second->FinalizeUpdate();
		for (auto& buf : m_ConstantBuffers)
			buf.second->Bind();
	}

	void DX11GeometryShader::UnBind() const
	{
		gfx->GetContext()->GSSetShader(nullptr, nullptr, 0u);

	}
	// recursivley access member in struct
	void ShaderDataStructAdd(ElementLayout* layout,ID3D11ShaderReflectionType* type, D3D11_SHADER_TYPE_DESC typedesc)
	{
		// if struct has member, loop and access all members
		for (UINT k = 0; k < typedesc.Members; ++k)
		{
			// get memberType and decide datatype
			ID3D11ShaderReflectionType* memberType = type->GetMemberTypeByIndex(k);
			D3D11_SHADER_TYPE_DESC memberTypedesc;
			memberType->GetDesc(&memberTypedesc);

			uint32_t memberOffset = memberTypedesc.Offset;
			// get member variable name
			std::string memberName = type->GetMemberTypeName(k);

			ShaderDataType memberDataType = DXShaderDataConvert(memberTypedesc);
			ShaderDataType  memberclassType = ShaderDataType::None;

			// if struct in array
			if (memberTypedesc.Elements > 0)
				memberclassType = ShaderDataType::Array;
			// if struct in struct
			else if (memberTypedesc.Members > 0)
				memberclassType = ShaderDataType::Struct;

			if (memberclassType == ShaderDataType::Struct)
			{
				// add struct in struct and do this proccess again
				(*layout).Add(memberclassType, memberName, memberOffset);
				ShaderDataStructAdd(&(*layout)[memberName], memberType, memberTypedesc);
			}
			else if (memberclassType == ShaderDataType::Array)
			{
				// add array in struct if struct array for adding member do this process again
				(*layout).Add(memberclassType, memberName, memberOffset);
				(*layout)[memberName].Set(memberDataType, memberTypedesc.Elements);
				if (memberDataType == ShaderDataType::Struct)
					ShaderDataStructAdd(&(*layout)[memberName].Array(), memberType, memberTypedesc);
			}
			else
			{
				(*layout).Add(memberDataType, memberName, memberOffset);
			}
		}
	}

	void DX11Shader::MakeBufferElement()
	{
		QCAT_PROFILE_FUNCTION();
		//======= Make Shader Element Buffer=========
		D3D11_SHADER_DESC desc;
		pReflector->GetDesc(&desc);
		UINT countConstantBuffer = desc.ConstantBuffers;
		for (UINT i = 0; i < countConstantBuffer; ++i)
		{
			ElementLayout layout(ShaderDataType::Struct);
			//std::vector<BufferElement> elements;
			std::string constantbuffername;
			
			// this is not com object dont worry about release
			ID3D11ShaderReflectionConstantBuffer* constantbuffer = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC shaderdesc;
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
				ShaderDataType classType = ShaderDataType::None;
				// if there is elements that is array
				if (typedesc.Elements > 0)
					classType = ShaderDataType::Array;
				// if member is more than zero that is struct
				else if(typedesc.Members>0)
					classType = ShaderDataType::Struct;
			
				// if Struct
				if (classType == ShaderDataType::Struct)
				{
					// add struct member in layout
					layout.Add(classType, name, offset, size);
					// after for adding member in struct , pass pointer of layout and reflection desc
					ShaderDataStructAdd(&layout[name], type, typedesc);
				}
				// if Array
				else if (classType == ShaderDataType::Array)
				{
					// add array member in layout
					layout.Add(classType, name, offset, size);
					// and Set layout with element's dataType(float,int..) and set count of element
					layout[name].Set(datatype, typedesc.Elements);
					// but when datatype is struct (meaning struct array) for adding member in struct pass pointer of layout and desc
					if(datatype == ShaderDataType::Struct)
						ShaderDataStructAdd(&layout[name].Array(), type, typedesc);
				}
				// if normal variable
				else
				{
					layout.Add(datatype, name, offset,size);
				}
			}
			Ref<DX11ConstantBuffer> pConstantBuffer;
			layout.CalculateSize(0);
			switch (type)
			{
			case DXshaderType::VS:
				pConstantBuffer = std::make_shared<VertexConstantBuffer>(layout, i);
				break;
			case DXshaderType::PS:
				pConstantBuffer = std::make_shared<PixelConstantBuffer>(layout, i);
				break;
			case DXshaderType::GS:
				pConstantBuffer = std::make_shared<GeometryConstantBuffer>(layout, i);
				break;
			}
			AddConstantBuffer(constantbuffername, pConstantBuffer);
		}
		//===========================================

		//for other Shader variable
	/*	for (int i = 0; i < desc.BoundResources; ++i)
		{

		}*/
	}

	void DX11Shader::AddConstantBuffer(const std::string& name, Ref<DX11ConstantBuffer>& pConstantBuffer)
	{
		QCAT_PROFILE_FUNCTION();

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
		QCAT_PROFILE_FUNCTION();

		auto iter = m_ConstantBuffers.find(name);
		if (iter == m_ConstantBuffers.end())
		{
			//QCAT_CORE_ERROR("Cant find vertexConstantBuffer! : Invalid name for buffer");
			return nullptr;
		}
		return iter->second;
	}

	
	DX11ComputeShader::DX11ComputeShader(const std::string& name, const std::string& path)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::CS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobGeo;
		D3DReadFileToBlob(ToWide(path).c_str(), &pBlobGeo);
		D3DReflect(pBlobGeo->GetBufferPointer(), pBlobGeo->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreateComputeShader(pBlobGeo->GetBufferPointer(),
			pBlobGeo->GetBufferSize(),
			nullptr, &pComputeShader);
	}

	DX11ComputeShader::DX11ComputeShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob)
	{
		QCAT_PROFILE_FUNCTION();

		type = DXshaderType::CS;
		m_name = name;
		this->gfx = QGfxDeviceDX11::GetInstance();
		D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
		//MakeBufferElement();
		HRESULT result = gfx->GetDevice()->CreateComputeShader(pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr, &pComputeShader);
	}

	DX11ComputeShader::~DX11ComputeShader()
	{
	}
	void DX11ComputeShader::Bind() const
	{
		gfx->GetContext()->CSSetShader(pComputeShader.Get(), nullptr, 0u);
	}
	void DX11ComputeShader::UpdateBuffer() const
	{
	}
	void DX11ComputeShader::UnBind() const
	{
		gfx->GetContext()->CSSetShader(nullptr,nullptr, 0u);
	}

}