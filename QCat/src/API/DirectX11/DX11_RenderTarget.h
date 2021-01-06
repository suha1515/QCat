#pragma once
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class QCAT_API DX11RenderTarget
	{
	public:
		DX11RenderTarget(QGfxDeviceDX11& gfx,uint32_t width,uint32_t height,DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		virtual ~DX11RenderTarget();

		void Bind(QGfxDeviceDX11& gfx,ID3D11DepthStencilView* pDepthStencilView = nullptr) const;
		void UnBind(QGfxDeviceDX11& gfx) const;

		void Clear(QGfxDeviceDX11& gfx,const glm::vec4& color);
		void* GetTexture() const ;
		void  SaveTexture();
	private:
		uint32_t m_width;
		uint32_t m_height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};
}