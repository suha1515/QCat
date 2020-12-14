#include "qcpch.h"
#include "Application.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <API/DirectX11/DX11_Shader.h>
#include <API/Opengl/OpenGLShader.h>

namespace wrl = Microsoft::WRL;
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}

namespace QCat
{
	Application* Application::instance = nullptr;
	Application::Application()
	{
		QCAT_CORE_ASSERT(!instance, "Application already exsists! ");
		instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		//m_window2 = std::unique_ptr<Window>(Window::Create());
		//m_window2->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImguiLayer = new ImGuiLayer();
		PushOverlay(m_ImguiLayer);

#if defined(QCAT_DX11)
		WindowsWindow* pWindow = dynamic_cast<WindowsWindow*>(m_window.get());
		QGfxDeviceDX11* pGfx = dynamic_cast<QGfxDeviceDX11*>(pWindow->Gfx());

		m_vertexShader.reset(new DX11VertexShader(*pGfx, "..\\bin\\Debug-windows-\\QCat\\Solid_VS.cso"));
		m_pixelShader.reset(new DX11PixelShader(*pGfx, "..\\bin\\Debug-windows-\\QCat\\Solid_PS.cso"));
		//vertex Array
		float vertices[] = {
			 0.0f, 0.5f,0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f,0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			-0.5f,-0.5f,0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		unsigned int stride = sizeof(float) * 7;
		//IndexArray
		unsigned int indices[3] = { 0,1,2 };

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices), &stride));
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		BufferLayout* buflayout = BufferLayout::Create(
			{ { ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" }, },
			m_vertexShader->GetData().data(),
			m_vertexShader->GetData().size()
		);
		m_VertexBuffer->SetLayout(buflayout);

		float squareVertices[3 * 4] = {
		-0.75f, -0.75f, 0.0f,
		-0.75f,  0.75f, 0.0f,
		 0.75f,  0.75f, 0.0f,
		 0.75f, -0.75f, 0.0f
		};
		stride = sizeof(float) * 3;

		unsigned squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareBuffer.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices), &stride));
		m_SquareIndex.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)));
		m_SquareVertexShader.reset(new DX11VertexShader(*pGfx, "..\\bin\\Debug-windows-\\QCat\\Square_VS.cso"));
		m_SquarePixelShader.reset(new DX11PixelShader(*pGfx, "..\\bin\\Debug-windows-\\QCat\\Square_PS.cso"));


		BufferLayout* squareLayout = BufferLayout::Create(
			{{ShaderDataType::Float3,"Position"}},
			m_SquareVertexShader->GetData().data(),
			m_SquareVertexShader->GetData().size()
		);

		m_SquareBuffer->SetLayout(squareLayout);

#elif defined(QCAT_OPENGL)
		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout* buflayout = BufferLayout::Create(
			{ { ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" } }
		);
		vertexBuffer->SetLayout(buflayout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0,1,2 };
		std::shared_ptr<IndexBuffer> indexbuffer;
		indexbuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexbuffer);


		m_SquareVA.reset(VertexArray::Create());
		float squareVertices[3 * 4] = {
		-0.75f, -0.75f, 0.0f,
		 0.75f, -0.75f, 0.0f,
		 0.75f,  0.75f, 0.0f,
		-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(BufferLayout::Create({ { ShaderDataType::Float3, "a_Position" } }));
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

			std::string vertexSrc = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;
				out vec3 v_Position;
				out vec4 v_Color;	

				void main()
				{
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = vec4(a_Position, 1.0);
				}
			)";

			std::string fragmentSrc = R"(
				#version 330 core

				layout(location = 0) out vec4 color;

				in vec3 v_Position;
				in vec4 v_Color;

				void main()
				{
					color = v_Color;
				}
			)";
			m_Shader.reset(new OpenGLShader(vertexSrc, fragmentSrc));

			std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

			std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

			m_BlueShader.reset(new OpenGLShader(blueShaderVertexSrc, blueShaderFragmentSrc));
#endif
	}

	Application::~Application()
	{
	
	}
	void Application::Run()
	{
		while (m_Running)
		{
			m_window->OnMessageUpdate();
			if (m_Running)
			{
				for (Layer* layer : m_layerStack)
					layer->OnUpdate();
				m_window->OnBegin();

#if defined(QCAT_DX11)
				WindowsWindow* pWindow = dynamic_cast<WindowsWindow*>(m_window.get());
				QGfxDeviceDX11* pGfx = dynamic_cast<QGfxDeviceDX11*>(pWindow->Gfx());
				pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				RenderCommand::SetClearColor(DirectX::XMFLOAT4{ 0.1f, 0.1f, 0.1f, 1 });
				RenderCommand::Clear();
				Renderer::BeginScene();
				m_SquareBuffer->Bind();
				m_SquareIndex->Bind();
				m_SquareVertexShader->Bind();
				m_SquarePixelShader->Bind();
				Renderer::Submit(m_SquareIndex->GetCount());
				m_VertexBuffer->Bind();
				m_IndexBuffer->Bind();
				m_vertexShader->Bind();
				m_pixelShader->Bind();
				Renderer::Submit(m_IndexBuffer->GetCount());
				Renderer::EndScene();

				D3D11_VIEWPORT vp;
				vp.Width = m_window->GetWidth();
				vp.Height = m_window->GetHeight();
				vp.MinDepth = 0;
				vp.MaxDepth = 1;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				pGfx->GetContext()->RSSetViewports(1u, &vp);		
#elif defined(QCAT_OPENGL)
				RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1 });
				RenderCommand::Clear();

				Renderer::BeginScene();
				m_BlueShader->Bind();
				Renderer::Submit(m_SquareVA);

				m_Shader->Bind();
				Renderer::Submit(m_VertexArray);
				
				Renderer::EndScene();
#endif			
					
				m_ImguiLayer->OnBegin();
				for (Layer* layer : m_layerStack)
					layer->OnImGuiRender();
				m_ImguiLayer->OnEnd();
				
				m_window->OnEnd();
			//	m_window2->OnBegin();

			//	m_window2->OnEnd();
			}
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
		//QCAT_CORE_INFO("{0}", e);
	}
	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_layerStack.PushOverlay(layer);
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}

