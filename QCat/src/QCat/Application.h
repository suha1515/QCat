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
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

		//InputLayout
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	
		// for opengl
		unsigned int m_vertexArray;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_vertexShader;
		std::unique_ptr<Shader> m_pixelShader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;


	private:
		static Application* instance;
	};

	// To be Defined in Client
	Application* CreateApplication();
}


