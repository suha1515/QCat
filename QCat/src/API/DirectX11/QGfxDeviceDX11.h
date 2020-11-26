#pragma once
#include <QCat/QCatCore.h>
#include <d3d11_3.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>


namespace QCat {

	class QCAT_API QGfxDeviceDX11
	{
	public:
		QGfxDeviceDX11(HWND hwnd, int width, int height, bool fullscreen = false);
		QGfxDeviceDX11(const QGfxDeviceDX11&) = delete;
		QGfxDeviceDX11& operator=(const QGfxDeviceDX11&) = delete;
		~QGfxDeviceDX11();

		void BeginFrame();
		void EndFrame();

	private:
		UINT width;
		UINT height;
	private:
		D3D_FEATURE_LEVEL fetureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	};
}
	

