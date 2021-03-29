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
			case DX11DepthStencil::Usage::Depth:
				return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
			}
		}
		DXGI_FORMAT MapUsageTyped(DX11DepthStencil::Usage usage)
		{
			switch (usage)
			{
			case DX11DepthStencil::Usage::DepthStencil:
				return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			case DX11DepthStencil::Usage::Depth:
				return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
			}
		}
		DXGI_FORMAT MapUsageColored(DX11DepthStencil::Usage usage)
		{
			switch (usage)
			{
			case DX11DepthStencil::Usage::DepthStencil:
				return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case DX11DepthStencil::Usage::Depth:
				return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			}
		}
	}
	DX11DepthStencil::DX11DepthStencil(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t arraySize, Usage usage, int samples,int quality, bool bindShader)
		:m_width(width), m_height(height),format(format),samples(samples),bindShader(bindShader),usage(usage),mipLevel(mipLevel),arraySize(arraySize),quality(quality)
	{
		textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = arraySize;
		textureDesc.Format = Utils::MapUsageTypeless(usage);
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = quality;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | (bindShader ? D3D11_BIND_SHADER_RESOURCE : 0);

		depthStencilViewDesc = {};
		depthStencilViewDesc.Format = Utils::MapUsageTyped(usage);
		depthStencilViewDesc.Flags = 0;
		if (textureDesc.ArraySize > 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthStencilViewDesc.Texture2DArray.MipSlice = 0;
			depthStencilViewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
		}
		else
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		

		srvDesc = {};
		srvDesc.Format = Utils::MapUsageColored(usage);
		
		if (textureDesc.ArraySize > 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
		}
		
		Initialize(gfx);
	}
	DX11DepthStencil::DX11DepthStencil(QGfxDeviceDX11& gfx, Usage usage,D3D11_TEXTURE2D_DESC textureDesc, D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
		:textureDesc(textureDesc),depthStencilViewDesc(depthStencilViewDesc),srvDesc(srvDesc),usage(usage)
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
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pDepthStencilTexture);
		gfx.GetDevice()->CreateDepthStencilView(pDepthStencilTexture.Get(), &depthStencilViewDesc, &pDepthStencilView);
	
		gfx.GetDevice()->CreateShaderResourceView(pDepthStencilTexture.Get(), &srvDesc, &pShaderResourceView);
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
		else if(usage == Usage::Depth)
			gfx.GetContext()->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH ,1.0f,0.0f);
	}
	ID3D11ShaderResourceView* DX11DepthStencil::GetTextureView() const
	{
		return pShaderResourceView.Get();
	}
	ID3D11Texture2D* DX11DepthStencil::GetTexture() const
	{
		return pDepthStencilTexture.Get();
	}
	ID3D11DepthStencilView* DX11DepthStencil::GetDepthStencil() const
	{
		return pDepthStencilView.Get();
	}
}