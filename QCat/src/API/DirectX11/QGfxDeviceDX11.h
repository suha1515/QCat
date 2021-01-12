#pragma once
#include <QCat/Core/QCatCore.h>
#include <QCat/Renderer/Graphics.h>
#include <d3d11_3.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d11shader.h>

#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

namespace QCat {

	class QCAT_API QGfxDeviceDX11 : public Graphics
	{
	public:
		static QGfxDeviceDX11* GetInstance()
		{
			if (Instance != nullptr)
			{
				return Instance;
			}
			return nullptr;
		};

	public:
		// inherit from Graphics
		virtual void Init(void* pHandle) override;
		virtual void Begin() override;
		virtual void End() override;
	public:
		QGfxDeviceDX11();
		QGfxDeviceDX11(const QGfxDeviceDX11&) = delete;
		QGfxDeviceDX11& operator=(const QGfxDeviceDX11&) = delete;
		~QGfxDeviceDX11();

		void Initialize(HWND hwnd, int width, int height, bool fullscreen = false);

		void BeginFrame(glm::vec4& color);
		void EndFrame();
		void Clear(glm::vec4& color);
	public:
		void CleanRenderTarget();
		void CreateRenderTarget();

		void SetRenderTarget();

		bool GetBlendState() { return m_BlendEnable; }
		void BlendStateEnable(bool enable);
		void BindBlendState();

		void SetSamplerState(D3D11_SAMPLER_DESC& samplerDesc);
		void BindSamplerState();
	public:
		ComPtr<ID3D11Device>& GetDevice();
		ComPtr<ID3D11DeviceContext>& GetContext();
		ComPtr<IDXGISwapChain1>& GetSwapChain();
	public:
		void SetViewPort(int width, int height);
		void SetWidthHeight(uint32_t width, uint32_t height);
	private:
		uint32_t width;
		uint32_t height;
	private:
		D3D_FEATURE_LEVEL fetureLevel;
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGISwapChain1> swapchain;
		ComPtr<ID3D11DeviceContext> immediateContext;

		ComPtr<ID3D11Texture2D> backBuffer;
		ComPtr<ID3D11Texture2D> depthStencil;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;

		Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlenderState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
		bool m_BlendEnable = false;
	private:
		static QGfxDeviceDX11* Instance;
	};
}
	

