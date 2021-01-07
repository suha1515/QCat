#pragma once
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11DepthStencil
	{
	public:
		DX11DepthStencil(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
		virtual ~DX11DepthStencil();
		
		void Initialize(QGfxDeviceDX11& gfx);
		void Bind(QGfxDeviceDX11& gfx) const;

		void Resize(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height);

		void Clear(QGfxDeviceDX11& gfx);
		ID3D11ShaderResourceView* GetTexture() const;
		ID3D11DepthStencilView* GetDepthStencil() const;
	private:
		uint32_t m_width;
		uint32_t m_height;
		DXGI_FORMAT format;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};
}