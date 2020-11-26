#include "qcpch.h"
#include "Application.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Log.h"

namespace QCat
{
	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());

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
}

