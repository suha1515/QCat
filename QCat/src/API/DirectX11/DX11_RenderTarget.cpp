#include "qcpch.h"
#include "DX11_RenderTarget.h"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image_write.h"
namespace QCat
{
	DX11RenderTarget::DX11RenderTarget(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, int samples, DXGI_FORMAT format)
		:m_width(width),m_height(height),format(format),samples(samples)
	{
		Initialize(gfx);
	}
	DX11RenderTarget::~DX11RenderTarget()
	{
	}
	void DX11RenderTarget::Initialize(QGfxDeviceDX11& gfx)
	{
		if (pTargetView)
		{
			pTargetView.Reset();
			pShaderResourceView.Reset();
		}
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

		gfx.GetDevice()->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &pTargetView);


		Microsoft::WRL::ComPtr<ID3D11Resource> pRes;
		pTargetView->GetResource(&pRes);
		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		gfx.GetDevice()->CreateShaderResourceView(pRes.Get(), &srvDesc, &pShaderResourceView);
	}
	void DX11RenderTarget::Bind(QGfxDeviceDX11& gfx,ID3D11DepthStencilView* pDepthStencilView) const
	{
		gfx.GetContext()->OMSetRenderTargets(1, pTargetView.GetAddressOf(), pDepthStencilView);
	}
	void DX11RenderTarget::UnBind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
	void DX11RenderTarget::Resize(QGfxDeviceDX11& gfx,uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		Initialize(gfx);
	}
	void DX11RenderTarget::Clear(QGfxDeviceDX11& gfx,const glm::vec4& color)
	{
		gfx.GetContext()->ClearRenderTargetView(pTargetView.Get(),glm::value_ptr(color));
	}
	void* DX11RenderTarget::GetTexture() const
	{
		return (void*)pShaderResourceView.Get();
	}
	void DX11RenderTarget::SaveTexture()
	{
		namespace wrl = Microsoft::WRL;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
		wrl::ComPtr<ID3D11Resource> pResSource;
		pShaderResourceView->GetResource(&pResSource);
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexSource->GetDesc(&textureDesc);
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;

		//텍스처 생성
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

		// 텍스처 복사
		gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx.GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		uint8_t* pSrcBytes = static_cast<uint8_t*>(msr.pData);
		stbi_write_png("test.png", m_width, m_height, 4, pSrcBytes, 4 * m_width);
		gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	}
}