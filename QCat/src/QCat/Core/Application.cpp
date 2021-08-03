#include "qcpch.h"
#include "Application.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Input.h"
#include "QCat/Renderer/Renderer.h"


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
	Application::Application(const std::string& name, RenderAPI::API api)
	{
		QCAT_PROFILE_FUNCTION();
		RenderAPI::SetRenderAPI(api);
		QCAT_CORE_ASSERT(!instance, "Application already exsists! ");
		instance = this;

		m_window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init(m_window->GetWidth(),m_window->GetHeight());

		m_ImguiLayer = new ImGuiLayer();
		PushOverlay(m_ImguiLayer);
	}

	Application::~Application()
	{
		QCAT_PROFILE_FUNCTION();

		//Renderer::Shutdown();
	}
	void Application::Run()
	{
		QCAT_PROFILE_FUNCTION();

		while (m_Running)
		{
			QCAT_PROFILE_SCOPE("RunLoop");
			Timestep timestep = timer.Mark();
			m_window->OnMessageUpdate();
			if (m_Running)
			{
				m_window->OnBegin();
				if (!m_minimized)
				{
					{
						QCAT_PROFILE_SCOPE("LayerStack OnUpdate");
 						for (Layer* layer : m_layerStack)
							layer->OnUpdate(timestep);
					}
					m_ImguiLayer->OnBegin();
					{
						QCAT_PROFILE_SCOPE("LayerStack OnImguiRender");
						for (Layer* layer : m_layerStack)
							layer->OnImGuiRender();
					}
					m_ImguiLayer->OnEnd();
				}					
				m_window->OnEnd();		
			}
		}
	}
	void Application::OnEvent(Event& e)
	{
		QCAT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		
		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend();++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
		//QCAT_CORE_INFO("{0}", e);
	}
	void Application::PushLayer(Layer* layer)
	{
		QCAT_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer)
	{
		QCAT_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		QCAT_PROFILE_FUNCTION();

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

