#pragma once
#include "QCatCore.h"
#include "Events/Event.h"
#include "QCat/Events/ApplicationEvent.h"

#include "Window.h"
namespace QCat
{
	class QCAT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		bool m_Running = true;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


