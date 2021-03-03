#include "qcpch.h"
#include "DX11_DepthStencil.h"

namespace QCat
{
	namespace Utils
	{
		DXGI_FORMAT MapUsageTypeless(DX11DepthStencil::Usage usage)
		{
			switch(usage)
			{
			case DX11DepthStencil::Usage::DepthStencil:
				return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
			case DX11DepthStencil::Usage::ShadowDepth:
				return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
			}
		}
		DXGI_FORMAT MapUsageTyped(DX11DepthStencil::Usage usage)
		{
			switch (usage)
			{
			case DX11DepthStencil::Usage::DepthStencil:
				return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			case DX11DepthStencil::Usage::ShadowDepth:
				return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
			}
		}
		DXGI_FORMAT MapUsageColored(DX11DepthStencil::Usage usage)
		{
			switch (usage)
			{
			case DX11DepthStencil::Usage::DepthStencil:
				return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case DX11DepthStencil::Usage::ShadowDepth:
				return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			}
		}
	}
	DX11DepthStencil::DX11DepthStencil(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, Usage usage, int samples, bool bindShader)
		:m_width(width), m_height(height),format(format),samples(samples),bindShader(bindShader),usage(usage)
	{
		Initialize(gfx);
	}
	DX11DepthStencil::~DX11DepthStencil()
	{
	}
	void DX11DepthStencil::Initialize(QGfxDeviceDX11& gfx)
	{
		if (pDepthStencilView)
		{
			pDepthStencilView.Reset();
			pShaderResourceView.Reset();
		}
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
		// Create Depth-Stencil Buffer
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = m_width;
		descDepth.Height = m_height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = Utils::MapUsageTypeless(usage);
		descDepth.SampleDesc.Count = samples;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		// TODO :split ShaderInput or not
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL| (bindShader? D3D11_BIND_SHADER_RESOURCE : 0);
		gfx.GetDevice()->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = Utils::MapUsageTyped(usage);
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;
		gfx.GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descView, &pDepthStencilView);

		// ShaderResorceView for DepthStencilView
		Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(&pRes);

		// TODO: split Format for usage
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapUsageColored(usage);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice()->CreateShaderResourceView(pRes.Get(), &srvDesc, &pShaderResourceView);
	}
	void DX11DepthStencil::Bind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
	}
	void DX11DepthStencil::UnBind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
	void DX11DepthStencil::Resize(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		Initialize(gfx);
	}
	void DX11DepthStencil::Clear(QGfxDeviceDX11& gfx)
	{
		if(usage == Usage::DepthStencil)
			gfx.GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
		else if(usage == Usage::ShadowDepth)
			gfx.GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH ,1.0f,0.0f);
	}
	ID3D11ShaderResourceView* DX11DepthStencil::GetTexture() const
	{
		return pShaderResourceView.Get();
	}
	ID3D11DepthStencilView* DX11DepthStencil::GetDepthStencil() const
	{
		return pDepthStencilView.Get();
	}
}