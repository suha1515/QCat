#include "qcpch.h"
#include "DX11_Shader.h"
#include "DX11_BufferLayout.h"

namespace QCat
{
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
	Ref<DX11VertexShader> DXShader::CreateVertexShaderFromNative(const std::string& name, const std::string& src)
	{
		QCAT_PROFILE_FUNCTION();
		//auto lastUnderbar = name.find_last_of('_');
		//std::string type = name.substr(lastUnderbar + 1, std::string::npos);
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(DXshaderType::VS, src);
		return std::make_shared<DX11VertexShader>(name, pBlob);
	}
	Ref<DX11PixelShader> DXShader::CreatePixelShaderFromNative(const std::string& name, const std::string& src)
	{
		QCAT_PROFILE_FUNCTION();

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		pBlob = Compile(DXshaderType::PS, src);
		return std::make_shared<DX11PixelShader>(name, pBlob);
	}
	Ref<DX11VertexShader> DXShader::CreateVertexShaderFromFile(const std::string& path)
	{
		QCAT_PROFILE_FUNCTION();

		const std::string name = GetShaderName(path);
		return std::make_shared<DX11VertexShader>(name, path);
	}
	Ref<DX11PixelShader> DXShader::CreatePixelShaderFromFile(const std::string& path)
	{
		QCAT_PROFILE_FUNCTION();

		auto name = GetShaderName(path);
		return std::make_shared<DX11PixelShader>(name, path);
	}
	Microsoft::WRL::ComPtr<ID3DBlob> DXShader::Compile(const DXshaderType& type, const std::string& src)
	{
		QCAT_PROFILE_FUNCTION();
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
	DXShader::DXShader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile)
		:m_name(name)
	{
		QCAT_PROFILE_FUNCTION();

		auto begin = vertexFile.find_last_of('.');

		std::string extension1 = vertexFile.substr(begin+1, vertexFile.length());
			begin = pixelFile.find_last_of('.');
		std::string extension2 = pixelFile.substr(begin+1, pixelFile.length());
		if (extension1 == "hlsl" && extension2 == "hlsl")
		{
			std::string vertexSrc = ReadFile(vertexFile);
			std::string pixelSrc = ReadFile(pixelFile);

			std::string vertexName = GetShaderName(vertexFile);
			std::string pixelName = GetShaderName(pixelFile);
			pvs = CreateVertexShaderFromNative(vertexName, vertexSrc);
			pps = CreatePixelShaderFromNative(pixelName, pixelSrc);
		}
		else if(extension1 == "cso" && extension2 == "cso")
		{
			pvs = CreateVertexShaderFromFile(vertexFile);
			pps = CreatePixelShaderFromFile(pixelFile);
		}
		else
		{
			QCAT_CORE_ASSERT(false, "Wrong extension! Shader Compile Error");
		}
		
	}
	DXShader::DXShader(const std::string& name, const std::string& vertexName, const std::string& vertexSrc, const std::string& pixelName, const std::string& pixelSrc, bool compile)
		: m_name(name)
	{
		QCAT_PROFILE_FUNCTION();

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
		QCAT_PROFILE_FUNCTION();

		pvs->Bind();
		pps->Bind();
	}
	void DXShader::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		pvs->UnBind();
		pps->UnBind();
	}
	void DXShader::SetInt(const std::string& name, int value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetIntArray(const std::string& name, int* values, unsigned int count)
	{
	}
	void DXShader::SetFloat(const std::string& name, const float& value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloatArray(const std::string name, float* values, unsigned int count)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		for (int i = 0; i < count; ++i)
		{
			ref.second[i] = values[i];
		}
	}
	void DXShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
		if (!ref.first)
			return;
		ref.first->IsDataChanged = true;
		ref.second = value;
	}
	void DXShader::SetBool(const std::string& name, const bool& value)
	{
		QCAT_PROFILE_FUNCTION();
		auto ref = FindVariable(name);
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
	std::pair<Ref<DX11ConstantBuffer>, ElementRef> DXShader::FindVariable(const std::string& name)
	{
		QCAT_PROFILE_FUNCTION();

		auto& vsConstantBuffers = pvs->GetConstantBuffers();
		std::pair<Ref<DX11ConstantBuffer>, ElementRef> pair;
		for (auto& iter : vsConstantBuffers)
		{
			auto ref = iter.second->FindVariable(name);
			if (ref.Exists())
				return { iter.second,ref };
		}
		auto& psConstantBuffers = pps->GetConstantBuffers();
		for (auto& iter : psConstantBuffers)
		{
			auto ref = iter.second->FindVariable(name);
			if (ref.Exists())
				return { iter.second,ref };
		}
		return{ nullptr, ElementRef{} };
	}
	//void DXShader::UpdateConstantBuffer(const std::string& uniformname, const::std::string& valuename, const void* pdata)
	//{
	//	/*QCAT_PROFILE_FUNCTION();

	//	bool suceed = true;
	//	Ref<DX11ConstantBuffer>& constantbuf = pvs->GetConstantBuffer(uniformname);
	//	if (constantbuf)
	//	{
	//		suceed = true;
	//		constantbuf->UpdateElement(valuename, pdata);
	//	}
	//	else
	//		suceed = false;

	//	constantbuf = pps->GetConstantBuffer(uniformname);
	//	if (constantbuf)
	//	{
	//		suceed = true;
	//		constantbuf->UpdateElement(valuename, pdata);
	//	}
	//	else
	//		suceed = false;
	//	if (!suceed)
	//	{
	//		QCAT_CORE_ERROR("{0} cant be find from all of {1}'s {2} constantbuffer", valuename,m_name, uniformname);
	//	}*/
	//}
	
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
	std::vector<char>& DXShader::GetVerexData()
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		return pvs->GetData();
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
		QCAT_PROFILE_FUNCTION();

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
		QCAT_PROFILE_FUNCTION();

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
		QCAT_PROFILE_FUNCTION();

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

		auto& iter = m_ConstantBuffers.find(name);
		if (iter == m_ConstantBuffers.end())
		{
			//QCAT_CORE_ERROR("Cant find vertexConstantBuffer! : Invalid name for buffer");
			return nullptr;
		}
		return iter->second;
	}

}