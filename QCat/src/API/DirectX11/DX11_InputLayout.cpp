#include "qcpch.h"
#include "DX11_InputLayout.h"

namespace QCat
{
	//Reference
		//InputLayout
		//D3D11_INPUT_ELEMENT_DESC desc[] =
		//{
		//	//½Ã¸àÆ½ ,ÀÎµ¦½º,Æ÷¸Ë		    ,ÀÎÇ²½½·Ô,¿ÀÇÁ¼Â,.. ÀÎ½ºÅÏ½Ì¿¡ »ç¿ëµÇ´Â°ª...
		//	{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//	{"Color",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0},
		//};
		//HRESULT result = pGfx->GetDevice()->CreateInputLayout(desc, 2,
		//	m_vertexShader->GetData().data(),
		//	m_vertexShader->GetData().size(),
		//	&pInputLayout);
	static DXGI_FORMAT ShaderDataTypeToDirectXType(ShaderDataType type)
	{
		switch (type)
		{
		case QCat::ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case QCat::ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case QCat::ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case QCat::ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case QCat::ShaderDataType::Mat3:     return DXGI_FORMAT_R32G32B32_FLOAT;
		case QCat::ShaderDataType::Mat4:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case QCat::ShaderDataType::Int:      return DXGI_FORMAT_R32_UINT;
		case QCat::ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_UINT;
		case QCat::ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_UINT;
		case QCat::ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_UINT;
		case QCat::ShaderDataType::Bool:     return DXGI_FORMAT_R32_UINT;
		}

		QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return DXGI_FORMAT_UNKNOWN;
	}
	DX11_InputLayout::DX11_InputLayout(const std::initializer_list<BufferElement>& elements, Ref<DXShader> vertexShader)
		:BufferLayout(elements)
	{
		for (auto& element : m_elements)
		{
			inputLayoutDesc.push_back({ element.name.c_str(),0,
				ShaderDataTypeToDirectXType(element.type),
				0,element.offset,D3D11_INPUT_PER_VERTEX_DATA,0 });
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateInputLayout(inputLayoutDesc.data(), m_elements.size()
			, vertexShader->GetVerexData().data(), vertexShader->GetVerexData().size(), &pInputLayout
		);
	}
	DX11_InputLayout::~DX11_InputLayout()
	{
	}
	void DX11_InputLayout::Bind() const
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->IASetInputLayout(pInputLayout.Get());
	}
	void DX11_InputLayout::UnBind() const
	{
	}
}