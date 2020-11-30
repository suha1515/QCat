#pragma once
#include <QCat/QCatCore.h>
#include <d3d11_3.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace Microsoft::WRL;

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
	public:
		void CleanRenderTarget();
		void CreateRenderTarget();
	public:
		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();
		ComPtr<IDXGISwapChain1>& GetSwapChain();
	public:
		void SetViewPort(int width, int height);
		
	private:
		UINT width;
		UINT height;
	private:
		D3D_FEATURE_LEVEL fetureLevel;
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGISwapChain1> swapchain;
		ComPtr<ID3D11DeviceContext> immediateContext;

		ComPtr<ID3D11Texture2D> backBuffer;
		ComPtr<ID3D11Texture2D> depthStencil;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
	};
}
	

