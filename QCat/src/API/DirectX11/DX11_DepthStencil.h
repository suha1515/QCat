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
			Depth,
		};
	public:
		DX11DepthStencil(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, Usage usage);
		DX11DepthStencil(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc,Usage usage);
		virtual ~DX11DepthStencil();
		
		void Initialize(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture);
		void Bind(QGfxDeviceDX11& gfx) const;
		void UnBind(QGfxDeviceDX11& gfx) const;

		void Clear(QGfxDeviceDX11& gfx);

		ID3D11DepthStencilView* GetDepthStencil() const;

		static Ref<DX11DepthStencil> Create(ID3D11Texture2D* pTexture, D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc, Usage usage=Usage::Depth);
	private:
		uint32_t m_width;
		uint32_t m_height;
	
		bool bindShader = false;
		Usage usage;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	};
}