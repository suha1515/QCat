#include "qcpch.h"
#include "Application.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Input.h"

#include "Renderer/Renderer.h"

#include <API/DirectX11/DX11_Shader.h>
#include <API/Opengl/OpenGLShader.h>
#include <glm/glm.hpp>

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
		:m_Camera(-1.6f,1.6f,-0.9f,0.9f)
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

		DX11Shader* dxShader = new DX11Shader("..\\bin\\Debug-windows-\\QCat\\Solid_VS.cso", "..\\bin\\Debug-windows-\\QCat\\Solid_PS.cso");
		m_Shader.reset(dxShader);

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
			dxShader->GetVerexData().data(),
			dxShader->GetVerexData().size()
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

		DX11Shader* squareShader = new DX11Shader("..\\bin\\Debug-windows-\\QCat\\Square_VS.cso", "..\\bin\\Debug-windows-\\QCat\\Square_PS.cso");
		m_BlueShader.reset(squareShader);

		BufferLayout* squareLayout = BufferLayout::Create(
			{{ShaderDataType::Float3,"Position"}},
			squareShader->GetVerexData().data(),
			squareShader->GetVerexData().size()
		);

		m_SquareBuffer->SetLayout(squareLayout);

		/*
			glm 과 DirectXMath 이 두 라이브러리에서 행렬의 차이점이있다.
			glm의 경우 columm 열우선 행렬로 데이터가 저장되고 XMMATRIX의 경우 row-major 즉 행우선으로 저장된다.

			하지만 제공하는 행렬변수가 데이터를 연속적으로 저장한다는뜻은아니다. 다만 쉐이더측에서 사용할때이다.

			행우선의 행렬을 사용할경우 아래와 같은 행렬의경우 
			v1	1   0  0  0
			v2	0   1  0  0
			v3	0   0  1  0
			v4	x   y  z  1 

			열우선 행렬의경우
			v1  v2 v3 v4

			1   0  0  x		로들어가게된다.
			0   1  0  y
			0   0  1  z
			0   0  0  1

			이두가지 종류는 곱하는셈시 다른 결과를 가져오게된다. 

			행우선의 경우 DirectXMath에서는 로컬*월드*뷰*투영 순으로 M * SRT * V* P 순으로 진행된다.

			열우선의 경우 Glm이나 Opengl에서는 투영 *뷰 *월드*로컬 순으로 P*V * TRS * M 순으로 진행된다.

			DirectX 나  glm 내부의 SSE를 사용하기에 빠른 곱셈을 사용하여 행렬곱이 구현되어있다.
			둘다 연속적인 메모리 공간을 차지하고있지만 행렬곱을 사용할때 다른요소끼리 곱하여 열,행 우선순위를 지킨다.

			결국 최종적인 행렬은 DirectX Matrix나 GLM이나 둘다 같은 행렬이 나올것이다 다만 서로 곱하는법이 다를뿐.
			최종결과의 행렬을 GLSL 던 HLSL 던 연속된메모리에 접근하여 쉐이더 내부 행렬변수를 패킹시 
			cpu 상의 (glm혹은directx의 행렬) 
			행렬의 [0][0] 는 쉐이더상에서 [0][0]
				   [0][1] 는 쉐이더상에서 [1][0]
				   [0][2] 는 쉐이더상에서 [2][0] ... 처럼 전치되서 쉐이더 행렬이 생성될것이다.
				  현재 쉐이더에서는 mul 곱하기 연산이 column-major 즉 열우선으로 되어있으므로. 따로 directx에서 전치하지 않아도 그대로 넣어주면된다.
				  아래 코드에서 행렬들의 값을 비교해보아라.. 연속된 값은 일정하지만. 순서를 바꿔 곱하면 값들이 달라지게된다.
				  glm의 열우선과 direct의 행우선사이에서 연산차이가 만들어내는것이다
				  도대체 왜 있는지 모르겟 --
		*/
		const glm::mat4& matrix = m_Camera.GetViewProjectionMatrix();
		glm::mat4 glmMatrix,glmviewMat,translateMat,rotationMat;
		glmMatrix  = glm::mat4(1.0f);
		glmviewMat = glm::mat4(1.0f);
		translateMat = glm::mat4(1.0f);
		rotationMat = glm::mat4(1.0f);
		glmMatrix = glm::orthoLH(-1.6f, 1.6f, -0.9f, 0.9f, -1.0f, 1.0f);
		translateMat = glm::translate(glm::mat4(1.0f), { 0.5f,0.5f,0.0f });
		rotationMat = glm::rotate(glm::mat4(1.0f), glm::radians(45.f), glm::vec3(0, 0, 1));
		glm::mat4 transform2 = translateMat* rotationMat;
	
		glmviewMat = glm::inverse(transform2);

		glmMatrix = glmMatrix * glmviewMat;
		auto xmMatrix = DirectX::XMFLOAT4X4(&glmMatrix[0][0]);
		DirectX::XMMATRIX mat33 = DirectX::XMLoadFloat4x4(&xmMatrix);
		

		DirectX::XMMATRIX mat;
		mat = DirectX::XMMatrixOrthographicOffCenterRH(-1.6f, 1.6f, -0.9f, 0.9f, 0.0f, 1.0f);
		DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.0f);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixIdentity();
		rotation = DirectX::XMMatrixRotationZ(glm::radians(45.f));
		DirectX::XMMATRIX transform =rotation* translate;
		DirectX::XMMATRIX viewMat = DirectX::XMMatrixInverse(nullptr,transform);
		mat = DirectX::XMMatrixOrthographicLH(3.2,1.8, 0.0f, 1.0f);
		mat = viewMat * mat;
		m_constantBuffer.reset(new DX11VertexConstantBuffer(*pGfx,0u, &matrix, sizeof(matrix)));

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

				uniform mat4 u_ViewProjection;

				out vec3 v_Position;
				out vec4 v_Color;	

				void main()
				{
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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
	
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
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
				Renderer::BeginScene(m_Camera);
				m_Camera.SetPosition({ 0.5f,0.5f,0.0f });
				m_Camera.SetRotation(m_rotation);
				m_constantBuffer->UpdateData(*pGfx, &m_Camera.GetViewProjectionMatrix());
				m_constantBuffer->Bind(*pGfx);
				m_SquareBuffer->Bind();
				m_SquareIndex->Bind();
				Renderer::Submit(m_BlueShader,m_SquareIndex->GetCount());
				m_VertexBuffer->Bind();
				m_IndexBuffer->Bind();
				Renderer::Submit(m_Shader,m_IndexBuffer->GetCount());
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

				m_Camera.SetPosition({ 0.5f,0.5f,0.0f });
				m_Camera.SetRotation(m_rotation);

				Renderer::BeginScene(m_Camera);
				Renderer::Submit(m_BlueShader,m_SquareVA);
				Renderer::Submit(m_Shader, m_VertexArray);
				
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
		dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(OnKeyPress));
		
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
	bool Application::OnKeyPress(KeyTypedEvent& e)
	{
		char character = e.GetKeyCode();
		if (character == 'r')
		{
			m_rotation += 5.f;
		}
		return true;
	}
}

