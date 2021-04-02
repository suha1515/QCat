#include "qcpch.h"
#include "DX11_DepthStencil.h"
#include "DXUtils.h"

namespace QCat
{
	DX11DepthStencil::DX11DepthStencil(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, Usage usage)
		:usage(usage)
	{
		D3D11_TEXTURE2D_DESC texdesc;
		pTexture->GetDesc(&texdesc);

		depthStencilViewDesc = {};
		depthStencilViewDesc.Format = Utils::MapTypeDSV(texdesc.Format);
		depthStencilViewDesc.Flags = 0;
		if (texdesc.ArraySize > 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilViewDesc.Texture2DArray.MipSlice = 0;
			depthStencilViewDesc.Texture2DArray.ArraySize = texdesc.ArraySize;
		}
		else
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}

		Initialize(gfx, pTexture);
	}

	DX11DepthStencil::DX11DepthStencil(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc, Usage usage)
		:depthStencilViewDesc(depthStencilDesc),usage(usage)
	{
		Initialize(gfx, pTexture);
	}
	
	DX11DepthStencil::~DX11DepthStencil()
	{
	}
	void DX11DepthStencil::Initialize(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture)
	{
		if (pDepthStencilView)
		{
			pDepthStencilView.Reset();
		}
		gfx.GetDevice()->CreateDepthStencilView(pTexture, &depthStencilViewDesc, &pDepthStencilView);
	}
	void DX11DepthStencil::Bind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
	}
	void DX11DepthStencil::UnBind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
	void DX11DepthStencil::Clear(QGfxDeviceDX11& gfx)
	{
		if(usage == Usage::DepthStencil)
			gfx.GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
		else if(usage == Usage::Depth)
			gfx.GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH ,1.0f,0.0f);
	}
	ID3D11DepthStencilView* DX11DepthStencil::GetDepthStencil() const
	{
		return pDepthStencilView.Get();
	}
}