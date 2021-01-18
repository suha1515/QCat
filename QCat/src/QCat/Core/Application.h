#pragma once
#include "QCatCore.h"
#include "Window.h"
#include "QCat/Events/Event.h"
#include "QCat/Layer/LayerStack.h"
#include "QCat/Events/ApplicationEvent.h"

#include "QCat/ImGui/ImGuiLayer.h"
#include <wrl.h>
#include "QCat/Uitiliy/Timestep.h"
#include "QCat/Uitiliy/Timer.h"

namespace QCat
{
	class QCAT_API Application
	{
	public:
		Application(const std::string& name="QCat App");
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& GetInstance() { return *instance; }
		inline Window* GetWindow() { return m_window.get(); }

		void Close() { m_Running = false; }

		ImGuiLayer* GetImguiLayer() { return m_ImguiLayer; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_window;
		ImGuiLayer* m_ImguiLayer;
		bool m_Running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;
		Timer timer;
	private:
		static Application* instance;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


