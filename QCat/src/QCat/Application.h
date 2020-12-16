#pragma once
#include "QCatCore.h"
#include "Window.h"
#include "Events/Event.h"
#include "QCat/Layer/LayerStack.h"
#include "QCat/Events/ApplicationEvent.h"

#include "QCat/ImGui/ImGuiLayer.h"
#include <wrl.h>
#include "API/DirectX11/QGfxDeviceDX11.h"
#include "API/DirectX11/DX11_Buffer.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

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
	private:
		static Application* instance;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


