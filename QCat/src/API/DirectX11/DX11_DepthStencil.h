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
		DX11DepthStencil(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height, uint32_t mipLevel, uint32_t arraySize,
						Usage usage,int samples = 1, int quality=0, bool bindShader = false);
		DX11DepthStencil(QGfxDeviceDX11& gfx, Usage usage, D3D11_TEXTURE2D_DESC textureDesc, D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);
		virtual ~DX11DepthStencil();
		
		void Initialize(QGfxDeviceDX11& gfx);
		void Bind(QGfxDeviceDX11& gfx) const;
		void UnBind(QGfxDeviceDX11& gfx) const;
		void Resize(QGfxDeviceDX11& gfx, uint32_t width, uint32_t height);

		void Clear(QGfxDeviceDX11& gfx);
		ID3D11ShaderResourceView* GetTextureView() const;
		ID3D11Texture2D* GetTexture() const;
		ID3D11DepthStencilView* GetDepthStencil() const;
	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t mipLevel;
		uint32_t arraySize;
		DXGI_FORMAT format;
		int samples;
		int quality;
		bool bindShader = false;
		Usage usage;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilTexture;

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	};
}