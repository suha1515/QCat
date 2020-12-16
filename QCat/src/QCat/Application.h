#pragma once
#include "QCatCore.h"
#include "Window.h"
#include "Events/Event.h"
#include "QCat/Layer/LayerStack.h"
#include "QCat/Events/ApplicationEvent.h"

#include "QCat/ImGui/ImGuiLayer.h"
#include <wrl.h>
#include "Uitiliy/Timestep.h"
#include "Uitiliy/Timer.h"

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

		inline static Application& GetInstance() { return *instance; }
		inline Window* GetWindow() { return m_window.get(); }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImguiLayer;
		bool m_Running = true;
		LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;
		Timer timer;
	private:
		static Application* instance;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


