#include "qcpch.h"
#include "Application.h"

#include "Events/KeyboardEvent.h"
#include "QCat/Log.h"


#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

void OnEvent2(QCat::Event& e)
{

}
namespace QCat
{
	Application::Application()
	{
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
			m_window->OnUpdate();
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		if (e.GetEventType() == EventType::KeyPressed)
		{
			QCAT_CORE_INFO("{0}", e);
		}
		QCAT_CORE_INFO("{0}", e);
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}

