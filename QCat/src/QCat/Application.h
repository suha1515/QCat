#pragma once
#include "QCatCore.h"
#include "Events/Event.h"
#include "QCat/Layer/LayerStack.h"
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

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		bool m_Running = true;
		LayerStack m_layerStack;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


