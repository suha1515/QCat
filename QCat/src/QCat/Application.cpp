#include "qcpch.h"
#include "Application.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Input.h"

#include "Renderer/Renderer.h"

namespace wrl = Microsoft::WRL;

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
		m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		//m_window2 = std::unique_ptr<Window>(Window::Create());
		//m_window2->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImguiLayer = new ImGuiLayer();
		PushOverlay(m_ImguiLayer);

#if defined(QCAT_DX11)
		
#elif defined(QCAT_OPENGL)
		
#endif
	}

	Application::~Application()
	{
	
	}
	void Application::Run()
	{
		while (m_Running)
		{
			Timestep timestep = timer.Mark();
			m_window->OnMessageUpdate();
			if (m_Running)
			{
				m_window->OnBegin();
				if (!m_minimized)
				{
					for (Layer* layer : m_layerStack)
						layer->OnUpdate(timestep);
					m_ImguiLayer->OnBegin();
					for (Layer* layer : m_layerStack)
						layer->OnImGuiRender();
					m_ImguiLayer->OnEnd();
				}
#if defined(QCAT_DX11)
		
#elif defined(QCAT_OPENGL)
				
#endif									
				m_window->OnEnd();
			//	m_window2->OnBegin();

			//	m_window2->OnEnd();
			}
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		
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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_minimized = true;
			return false;
		}
		m_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
	
}

