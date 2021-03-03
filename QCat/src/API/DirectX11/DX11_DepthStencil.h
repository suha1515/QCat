#pragma once
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11DepthStencil
	{
	public:
		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};
	public:
		DX11DepthStencil(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, Usage usage,int samples = 1, bool bindShader = false);
		virtual ~DX11DepthStencil();
		
		void Initialize(QGfxDeviceDX11& gfx);
		void Bind(QGfxDeviceDX11& gfx) const;
		void UnBind(QGfxDeviceDX11& gfx) const;
		void Resize(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height);

		void Clear(QGfxDeviceDX11& gfx);
		ID3D11ShaderResourceView* GetTexture() const;
		ID3D11DepthStencilView* GetDepthStencil() const;
	private:
		uint32_t m_width;
		uint32_t m_height;
		DXGI_FORMAT format;
		int samples;
		bool bindShader = false;
		Usage usage;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	};
}