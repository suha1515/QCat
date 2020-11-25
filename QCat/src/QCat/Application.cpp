#include "Application.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Log.h"

namespace QCat
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			QCAT_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			QCAT_TRACE(e);
		}
		while (true);
	}
}

