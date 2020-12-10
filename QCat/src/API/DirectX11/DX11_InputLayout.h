#pragma once
#include <Renderer/Buffer.h>
#include <API/DirectX11/QGfxDeviceDX11.h>

namespace QCat
{
	class QCAT_API DX11_InputLayout : public BufferLayout
	{
	public:
		DX11_InputLayout(const std::initializer_list<BufferElement>& elements,void* vertexShaderCode,unsigned int codeSize);
		~DX11_InputLayout();
	public:
		virtual void Bind()const;
		virtual void UnBind()const;

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

	};
}
