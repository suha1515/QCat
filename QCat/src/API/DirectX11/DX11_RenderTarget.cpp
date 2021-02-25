#include "qcpch.h"
#include "DX11_RenderTarget.h"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image_write.h"
namespace QCat
{
	DX11RenderTarget::DX11RenderTarget(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
		:rtvDesc(rtvDesc)
	{
		Initialize(gfx, pTexture);
	}
	DX11RenderTarget::~DX11RenderTarget()
	{
	}
	void DX11RenderTarget::Initialize(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture)
	{
		if (pTargetView)
		{
			pTargetView.Reset();
		}
		D3D11_TEXTURE2D_DESC textureDesc;
		// Create Texture
		pTexture->GetDesc(&textureDesc);
		m_width = textureDesc.Width;
		m_height = textureDesc.Height;

		// Create Render Target View
		gfx.GetDevice()->CreateRenderTargetView(pTexture, &rtvDesc, &pTargetView);
	}
	void DX11RenderTarget::Bind(QGfxDeviceDX11& gfx,ID3D11DepthStencilView* pDepthStencilView) const
	{
		gfx.GetContext()->OMSetRenderTargets(1, pTargetView.GetAddressOf(), pDepthStencilView);
	}
	void DX11RenderTarget::UnBind(QGfxDeviceDX11& gfx) const
	{
		gfx.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
	//void DX11RenderTarget::ReadData(uint32_t x, uint32_t y,int* outdata)
	//{
	//	namespace wrl = Microsoft::WRL;

	//	QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
	//	// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
	//	wrl::ComPtr<ID3D11Resource> pResSource;
	//	pShaderResourceView->GetResource(&pResSource);
	//	wrl::ComPtr<ID3D11Texture2D> pTexSource;
	//	pResSource.As(&pTexSource);
	//	D3D11_TEXTURE2D_DESC textureDesc;
	//	textureDesc.Width = 1;
	//	textureDesc.Height = 1;
	//	textureDesc.MipLevels = 1;
	//	textureDesc.ArraySize = 1;
	//	textureDesc.Format = format;
	//	textureDesc.SampleDesc.Count = samples;
	//	textureDesc.SampleDesc.Quality = 0;
	//	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//	textureDesc.Usage = D3D11_USAGE_STAGING;
	//	textureDesc.BindFlags = 0;
	//	textureDesc.MiscFlags = 0;
	//	wrl::ComPtr<ID3D11Texture2D> pTexTemp;

	//	//텍스처 생성
	//	gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

	//	// 텍스처 복사
	//	//gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
	//	D3D11_BOX srcBox;
	//	srcBox.left = x;
	//	srcBox.right = x + 1;
	//	srcBox.bottom = y + 1;
	//	srcBox.top = y;
	//	srcBox.front = 0;
	//	srcBox.back = 1;
	//	gfx.GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), 0, &srcBox);

	//	D3D11_MAPPED_SUBRESOURCE msr = {};
	//	gfx.GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
	//	if (x >= 0 && x <= m_width && y >= 0 && y <= m_height)
	//	{
	//		int* data = reinterpret_cast<int*>(msr.pData);
	//		*outdata = *data;
	//	}
	//	gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	//}
	void DX11RenderTarget::Clear(QGfxDeviceDX11& gfx,const glm::vec4& color)
	{
		gfx.GetContext()->ClearRenderTargetView(pTargetView.Get(),glm::value_ptr(color));
	}
	//void DX11RenderTarget::SaveTexture(ID3D11ShaderResourceView** pShaderResourceView)
	//{
	//	namespace wrl = Microsoft::WRL;

	//	QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
	//	// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
	//	wrl::ComPtr<ID3D11Resource> pResSource;
	//	(*pShaderResourceView)->GetResource(&pResSource);
	//	wrl::ComPtr<ID3D11Texture2D> pTexSource;
	//	pResSource.As(&pTexSource);
	//	D3D11_TEXTURE2D_DESC textureDesc;
	//	pTexSource->GetDesc(&textureDesc);
	//	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//	textureDesc.Usage = D3D11_USAGE_STAGING;
	//	textureDesc.BindFlags = 0;
	//	wrl::ComPtr<ID3D11Texture2D> pTexTemp;

	//	//텍스처 생성
	//	gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

	//	// 텍스처 복사
	//	gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
	//	D3D11_MAPPED_SUBRESOURCE msr = {};
	//	gfx.GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
	//	uint8_t* pSrcBytes = static_cast<uint8_t*>(msr.pData);
	//	stbi_write_png("test.png", textureDesc.Width, textureDesc.Height, 4, pSrcBytes, 4 * textureDesc.Width);
	//	gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	//}
	Ref<DX11RenderTarget> DX11RenderTarget::Create(ID3D11Texture2D* pTexture, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc)
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		return CreateRef<DX11RenderTarget>(gfx,pTexture,rtvDesc);
	}
}