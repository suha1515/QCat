#include "qcpch.h"
#include "Application.h"

#include "Events/KeyboardEvent.h"
#include "QCat/Log.h"

#include "Input.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace QCat
{

	Application* Application::instance = nullptr;
	Application::Application()
	{
		QCAT_CORE_ASSERT(!instance, "Application already exsists! ");
		instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
		
	}
	void Application::Run()
	{
		while (m_Running)
		{
			m_window->OnBegin();
			for (Layer* layer : m_layerStack)
				layer->OnUpdate();
			
			if (Input::IsKeyPressded(QCAT_KEY_0))
			{
				QCAT_CORE_INFO("Key Clicked!");
			}
			//auto [x, y] = Input::GetMousePos();
			//QCAT_CORE_TRACE("{0} , {1}", x, y);

			m_window->OnEnd();
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

