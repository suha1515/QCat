#include "qcpch.h"
#include "DX11_RenderAPI.h"
#include "API/DirectX11/QGfxDeviceDX11.h"

namespace QCat
{
	DX11RenderAPI::DX11RenderAPI()
		:color(glm::vec4(0.0f))
	{
 		pgfx = nullptr;		
	}
	DX11RenderAPI::~DX11RenderAPI()
	{
	}
	void DX11RenderAPI::Init()
	{
		QCAT_PROFILE_FUNCTION();
		pgfx = QGfxDeviceDX11::GetInstance();
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		pgfx->BlendStateEnable(true);
		pgfx->SetSamplerState(samplerDesc);
		pgfx->BindSamplerState();
	}
	void DX11RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		pgfx->CleanRenderTarget();
		pgfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		pgfx->CreateRenderTarget();
		pgfx->SetViewPort(width, height);
	}
	void DX11RenderAPI::SetDefaultFrameBuffer()
	{
		pgfx->SetRenderTarget();
	}
	void DX11RenderAPI::SetClearColor(const glm::vec4& color)
	{
		this->color = color;
	}
	void DX11RenderAPI::Clear()
	{
		pgfx->Clear(color);
	}
	void DX11RenderAPI::DrawIndexed(const unsigned int indexCount)
	{
		if (pgfx->GetBlendState())
		{
			pgfx->BindBlendState();
		}
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
	void DX11RenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount)
	{
		unsigned int count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		DrawIndexed(count);
		//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullSRV =  nullptr;
		//pgfx->GetContext()->PSSetShaderResources(0u, 1u, nullSRV.GetAddressOf());
	}
}