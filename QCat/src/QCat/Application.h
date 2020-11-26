#pragma once
#include "QCatCore.h"
#include "Events/Event.h"
#include "Window.h"

namespace QCat
{
	class QCAT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_window;
		bool m_Running = true;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


