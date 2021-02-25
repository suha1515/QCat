#pragma once
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class QCAT_API DX11RenderTarget
	{
	public:
		DX11RenderTarget(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture, D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {});
		virtual ~DX11RenderTarget();

		void Initialize(QGfxDeviceDX11& gfx, ID3D11Texture2D* pTexture);

		void Bind(QGfxDeviceDX11& gfx,ID3D11DepthStencilView* pDepthStencilView = nullptr) const;
		void UnBind(QGfxDeviceDX11& gfx) const;


		void Clear(QGfxDeviceDX11& gfx,const glm::vec4& color);
		ID3D11RenderTargetView* GetRenderTargetView() { return pTargetView.Get(); }
		static Ref<DX11RenderTarget> Create(ID3D11Texture2D* pTexture,D3D11_RENDER_TARGET_VIEW_DESC rtvDesc);
	private:
		uint32_t m_width;
		uint32_t m_height;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTargetView;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	};
}