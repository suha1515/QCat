#pragma once
#include "QCatCore.h"
#include "Window.h"
#include "Events/Event.h"
#include "QCat/Layer/LayerStack.h"
#include "QCat/Events/ApplicationEvent.h"

#include "QCat/ImGui/ImGuiLayer.h"
#include <wrl.h>
#include "API/DirectX11/QGfxDeviceDX11.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

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
		std::unique_ptr<Window> m_window2;
		ImGuiLayer* m_ImguiLayer;
		bool m_Running = true;
		LayerStack m_layerStack;

		//temperary code for Testing
	private:	
		std::unique_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;

		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;

		std::unique_ptr<VertexBuffer> m_SquareBuffer;
		std::unique_ptr<IndexBuffer> m_SquareIndex;

		std::unique_ptr<Shader> m_SquareVertexShader;
		std::unique_ptr<Shader> m_SquarePixelShader;
	private:
		static Application* instance;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


