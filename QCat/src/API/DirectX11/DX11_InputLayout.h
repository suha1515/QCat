#pragma once
#include <QCat/Renderer/Buffer.h>
#include <API/DirectX11/DX11_Shader.h>
#include <API/DirectX11/QGfxDeviceDX11.h>

namespace QCat
{
	class QCAT_API DX11_InputLayout : public BufferLayout
	{
	public:
		struct VertexShaderData
		{
			void* vertexShaderCode;
			unsigned int codeSize;
		};
	public:
		DX11_InputLayout(const std::initializer_list<BufferElement>& elements, Ref<DXShader> vertexShader);
		~DX11_InputLayout();
	public:
		virtual void Bind()const;
		virtual void UnBind()const;

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

	};
}
