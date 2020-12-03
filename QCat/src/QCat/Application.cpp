#include "qcpch.h"
#include "Application.h"

#include "Events/KeyboardEvent.h"
#include "QCat/Log.h"

#include  "Platform/Windows/WindowsWindow.h"
#include "Input.h"

namespace wrl = Microsoft::WRL;
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
std::wstring ToWide(const std::string& narrow)
{
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
	return wide;
}

namespace QCat
{

	Application* Application::instance = nullptr;
	Application::Application()
	{
		QCAT_CORE_ASSERT(!instance, "Application already exsists! ");
		instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImguiLayer = new ImGuiLayer();
		PushOverlay(m_ImguiLayer);


		WindowsWindow* pWindow = dynamic_cast<WindowsWindow*>(m_window.get());
		QGfxDeviceDX11* pGfx = dynamic_cast<QGfxDeviceDX11*>(pWindow->Gfx());

		//vertex Array
		float vertices[] = {
			 0.f, 0.5f,0.0f,
			 0.5f, -0.5f,0.0f,
			 -0.5f,-0.5f,0.0f
		};

		//vertex Buffer
		UINT stride = sizeof(float)*3;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(vertices));
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = &vertices;
		const UINT offset = 0u;
		pGfx->GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer);
		
	
		//IndexArray
		unsigned short indices[3] = { 0,1,2 };
		//Index Buffer
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = UINT(sizeof(indices));
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = &indices;
		pGfx->GetDevice()->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		//Vertex Shader
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobVertex;
		D3DReadFileToBlob(ToWide("..\\bin\\Debug-windows-\\QCat\\Solid_VS.cso").c_str(), &pBlobVertex);

		pGfx->GetDevice()->CreateVertexShader(pBlobVertex->GetBufferPointer(),
			pBlobVertex->GetBufferSize(),
			nullptr, &pVertexShader);

		//Input Layout
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		pGfx->GetDevice()->CreateInputLayout(desc, 1,
			pBlobVertex->GetBufferPointer()
			, pBlobVertex->GetBufferSize(),
			&pInputLayout);

		//PixelShader
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobPixel;
		D3DReadFileToBlob(ToWide("..\\bin\\Debug-windows-\\QCat\\Solid_PS.cso").c_str(), &pBlobPixel);
		pGfx->GetDevice()->CreatePixelShader(pBlobPixel->GetBufferPointer(),
			pBlobPixel->GetBufferSize(),
			nullptr, &pPixelShader);
	}

	Application::~Application()
	{
	
	}
	void Application::Run()
	{
		while (m_Running)
		{
			m_window->OnMessageUpdate();
			if (m_Running)
			{
				m_window->OnBegin();
				for (Layer* layer : m_layerStack)
					layer->OnUpdate();

				WindowsWindow* pWindow = dynamic_cast<WindowsWindow*>(m_window.get());
				QGfxDeviceDX11* pGfx = dynamic_cast<QGfxDeviceDX11*>(pWindow->Gfx());
				//// Topology
				unsigned int stride = sizeof(float)*3;
				const UINT offset = 0u;
				pGfx->GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
				pGfx->GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

				pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				pGfx->GetContext()->IASetInputLayout(pInputLayout.Get());

				pGfx->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);

				pGfx->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
				pGfx->GetContext()->DrawIndexed(3u, 0u, 0u);
				D3D11_VIEWPORT vp;
				vp.Width = m_window->GetWidth();
				vp.Height = m_window->GetHeight();
				vp.MinDepth = 0;
				vp.MaxDepth = 1;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				pGfx->GetContext()->RSSetViewports(1u, &vp);
				//pGfx->GetContext()->Draw(3u,0u);

				m_ImguiLayer->OnBegin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiRender();
				m_ImguiLayer->OnEnd();

				m_window->OnEnd();
			}
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		//QCAT_CORE_INFO("{0}", e);
	}
	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_layerStack.PushOverlay(layer);
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}

