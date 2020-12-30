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
		QGfxDeviceDX11* gfx = QGfxDeviceDX11::GetInstance();
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		gfx->BlendStateEnable(true);
		gfx->SetSamplerState(samplerDesc);
		gfx->BindSamplerState();
	}
	void DX11RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		QGfxDeviceDX11* gfx = QGfxDeviceDX11::GetInstance();
		QCAT_CORE_ASSERT(gfx != nullptr, "cast Error!");

		gfx->CleanRenderTarget();
		gfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		gfx->CreateRenderTarget();
		gfx->SetViewPort(width, height);
	}
	void DX11RenderAPI::SetClearColor(const glm::vec4& color)
	{
		this->color = color;
	}
	void DX11RenderAPI::Clear()
	{
		if(pgfx==nullptr)
			pgfx = QGfxDeviceDX11::GetInstance();
		pgfx->BeginFrame(color);
	}
	void DX11RenderAPI::DrawIndexed(const unsigned int indexCount)
	{
		if (pgfx == nullptr)
			pgfx = QGfxDeviceDX11::GetInstance();
		if (pgfx->GetBlendState())
		{
			pgfx->BindBlendState();
		}
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
	void DX11RenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		DrawIndexed(vertexArray->GetIndexBuffer()->GetCount());
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullSRV =  nullptr;
		//pgfx->GetContext()->PSSetShaderResources(0u, 1u, nullSRV.GetAddressOf());
	}
}