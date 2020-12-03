#include "qcpch.h"
#include "QGfxDeviceDX11.h"
#include "QCat/Log.h"


namespace QCat {
	void QGfxDeviceDX11::Init(void* pHandle)
	{
		HWND hwnd = static_cast<HWND>(pHandle);
		QCAT_CORE_ASSERT(hwnd != NULL, "window handle is NULL!");

		RECT rect;
		GetClientRect(hwnd, &rect);
		
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		Initialize(hwnd, width, height);
	}
	void QGfxDeviceDX11::Begin()
	{
		BeginFrame();
	}
	void QGfxDeviceDX11::End()
	{
		EndFrame();
	}

	QGfxDeviceDX11::QGfxDeviceDX11()
	{
		height = 0, width = 0;
	}
	void QGfxDeviceDX11::Initialize(HWND hwnd, int width, int height,bool fullscreen)
	{
		UINT createDeviceFlags = 0;
	#ifdef QCAT_DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		// feacture level
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		uint32_t numFeatureLevels = ARRAYSIZE(featureLevels);
		HRESULT hr = E_FAIL;
		hr = D3D11CreateDevice(
			nullptr, D3D_DRIVER_TYPE_HARDWARE,
			0, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &device, &fetureLevel, &immediateContext);

		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create DirectX11 graphics device! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}
		
		// DXGI device,adapter,factory
		Microsoft::WRL::ComPtr<IDXGIDevice2> pDXGIDevice;
		hr = device->QueryInterface(__uuidof(IDXGIDevice2), (void**)&pDXGIDevice);

		Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;
		hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

		// To get specific information about Graphic Card
		//DXGI_ADAPTER_DESC adapterDesc;
		//pDXGIAdapter->GetDesc(&adapterDesc);	

		Microsoft::WRL::ComPtr<IDXGIFactory2> pIDXGIFactory;
		hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pIDXGIFactory);

		int adapterCount = 0;
		IDXGIAdapter* pAdapter;
		for (UINT i = 0; pIDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC adapterDesc;
			IDXGIOutput* pOutput;
			pAdapter->GetDesc(&adapterDesc);
			if (pAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice),NULL)!= DXGI_ERROR_UNSUPPORTED)
			{
				std::wstring wgrapics = adapterDesc.Description;
				std::string graphics; graphics.assign(wgrapics.begin(), wgrapics.end());
				UINT i = 0;
				while (pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
				{
					DXGI_OUTPUT_DESC outputDesc;
					pOutput->GetDesc(&outputDesc);
					std::wstring wmonitorname = outputDesc.DeviceName;
					std::string  monitername;
					monitername.assign(wmonitorname.begin(), wmonitorname.end());
					QCAT_CORE_INFO("{0} adapeter is {1}  support DX  and monitor {2}", adapterCount, graphics, monitername);
					++i;
					pOutput->Release();
				}
			}
			else
			{
				std::wstring wgrapics = adapterDesc.Description;
				std::string graphics; graphics.assign(wgrapics.begin(), wgrapics.end());
				UINT i = 0;
				while (pAdapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
				{
					DXGI_OUTPUT_DESC outputDesc;
					pOutput->GetDesc(&outputDesc);
					std::wstring wmonitorname = outputDesc.DeviceName;
					std::string  monitername;
					monitername.assign(wmonitorname.begin(), wmonitorname.end());
					QCAT_CORE_INFO("{0} adapeter is {1} doesnt support DX and monitor {2}", adapterCount, graphics, monitername);
					++i;
					pOutput->Release();
				}
			}
			adapterCount++;
			pAdapter->Release();
		}

			


		//Swap Chain
		// https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model
		// flip -model
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.Stereo = false;
		sd.SampleDesc.Count =  1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2;
		sd.Flags = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		sd.Scaling = DXGI_SCALING_NONE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
		fullscreenDesc.RefreshRate.Numerator = 60;
		fullscreenDesc.RefreshRate.Numerator = 1;
		fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		fullscreenDesc.Windowed = !fullscreen;

		hr = pIDXGIFactory->CreateSwapChainForHwnd(device.Get(), hwnd, &sd, &fullscreenDesc, nullptr, &swapchain);

		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create SwapChain for gfx device! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		hr = pDXGIDevice->SetMaximumFrameLatency(1);

		//Create BackBuffer
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		hr = swapchain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create BackBuffer ! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}

		hr = device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &renderTargetView);
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create RenderTarget ! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}
		immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), 0);

		// Create Depth-Stencil Buffer
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = device->CreateTexture2D(&descDepth, nullptr, &depthStencil);
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create DepthStencil Texture ! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencil.Get(), nullptr, &depthStencilView);
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Failed to create DepthStencil View ! Error code: " << std::hex << hr;
			QCAT_CORE_ERROR(ss.str());
		}

		//Create ViewPort
		D3D11_VIEWPORT vp;
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1u, &vp);
	}

	QGfxDeviceDX11::~QGfxDeviceDX11()
	{
	}
	void QGfxDeviceDX11::BeginFrame()
	{
		std::array<float, 4> color = { 1.0f,0.0f,1.0f,0.0f };
		immediateContext->ClearRenderTargetView(renderTargetView.Get(), color.data());
		immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);
		immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	void QGfxDeviceDX11::EndFrame()
	{
		swapchain->Present(1u,0);
	}
	void QGfxDeviceDX11::CleanRenderTarget()
	{
		if (renderTargetView)
		{
			renderTargetView.Reset();
		}
	}
	void QGfxDeviceDX11::CreateRenderTarget()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
		//swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
		if(renderTargetView==nullptr)
			device->CreateRenderTargetView(pBackBuffer.Get(), NULL, &renderTargetView);
	}
	ComPtr<ID3D11Device>& QGfxDeviceDX11::GetDevice()
	{
		return device;
	}
	ComPtr<ID3D11DeviceContext>& QGfxDeviceDX11::GetContext()
	{
		return immediateContext;
	}
	ComPtr<IDXGISwapChain1>& QGfxDeviceDX11::GetSwapChain()
	{
		return swapchain;
	}
	void QGfxDeviceDX11::SetViewPort(int width, int height)
	{
		D3D11_VIEWPORT vp;
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1u, &vp);
	}
}


