#include <QCat.h>

class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example"),m_Camera(-1.6f,1.6f,-0.9f,0.9f)
	{
		m_CameraPosition = glm::vec3(0.0f);
#if defined(QCAT_DX11)
		QCat::DX11Shader* dxShader = new QCat::DX11Shader("..\\bin\\Debug-windows-\\QCat\\Solid_VS.cso", "..\\bin\\Debug-windows-\\QCat\\Solid_PS.cso");
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

		m_VertexBuffer.reset(QCat::VertexBuffer::Create(vertices, sizeof(vertices), &stride));
		m_IndexBuffer.reset(QCat::IndexBuffer::Create(indices, sizeof(indices)));
		QCat::BufferLayout* buflayout = QCat::BufferLayout::Create(
			{ { QCat::ShaderDataType::Float3, "Position" },
			{ QCat::ShaderDataType::Float4, "Color" }, },
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

		m_SquareBuffer.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices), &stride));
		m_SquareIndex.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices)));

		QCat::DX11Shader* squareShader = new QCat::DX11Shader("..\\bin\\Debug-windows-\\QCat\\Square_VS.cso", "..\\bin\\Debug-windows-\\QCat\\Square_PS.cso");
		m_BlueShader.reset(squareShader);

		QCat::BufferLayout* squareLayout = QCat::BufferLayout::Create(
			{ {QCat::ShaderDataType::Float3,"Position"} },
			squareShader->GetVerexData().data(),
			squareShader->GetVerexData().size()
		);

		m_SquareBuffer->SetLayout(squareLayout);

		const glm::mat4& matrix = m_Camera.GetViewProjectionMatrix();
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		m_constantBuffer.reset(new QCat::DX11VertexConstantBuffer(*pGfx, 0u, &matrix, sizeof(matrix)));

#elif defined(QCAT_OPENGL)
		m_VertexArray.reset(QCat::VertexArray::Create());

		float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<QCat::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(QCat::VertexBuffer::Create(vertices, sizeof(vertices)));

		QCat::BufferLayout* buflayout = QCat::BufferLayout::Create(
			{ { QCat::ShaderDataType::Float3, "a_Position" },
				{ QCat::ShaderDataType::Float4, "a_Color" } }
		);
		vertexBuffer->SetLayout(buflayout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0,1,2 };
		std::shared_ptr<QCat::IndexBuffer> indexbuffer;
		indexbuffer.reset(QCat::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexbuffer);


		m_SquareVA.reset(QCat::VertexArray::Create());
		float squareVertices[3 * 4] = {
		-0.75f, -0.75f, 0.0f,
		 0.75f, -0.75f, 0.0f,
		 0.75f,  0.75f, 0.0f,
		-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<QCat::VertexBuffer> squareVB;
		squareVB.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(QCat::BufferLayout::Create({ { QCat::ShaderDataType::Float3, "a_Position" } }));
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<QCat::IndexBuffer> squareIB;
		squareIB.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
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
		m_Shader.reset(new QCat::OpenGLShader(vertexSrc, fragmentSrc));

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

		m_BlueShader.reset(new QCat::OpenGLShader(blueShaderVertexSrc, blueShaderFragmentSrc));
#endif	
	}
	void OnUpdate() override
	{
		if (QCat::Input::IsKeyPressed(VK_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed;
		else if (QCat::Input::IsKeyPressed(VK_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed;

		if (QCat::Input::IsKeyPressed(VK_UP))
			m_CameraPosition.y += m_CameraMoveSpeed;
		else if (QCat::Input::IsKeyPressed(VK_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed;

		if (QCat::Input::IsKeyPressed('A'))
			m_CameraRoatation += m_CameraRoatationSpeed;
		if (QCat::Input::IsKeyPressed('D'))
			m_CameraRoatation -= m_CameraRoatationSpeed;



		QCat::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1 });
		QCat::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRoatation);

		QCat::Renderer::BeginScene(m_Camera);
#ifdef QCAT_DX11
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_constantBuffer->UpdateData(*pGfx, &m_Camera.GetViewProjectionMatrix());
		m_constantBuffer->Bind(*pGfx);
		m_SquareBuffer->Bind();
		m_SquareIndex->Bind();
		QCat::Renderer::Submit(m_BlueShader, m_SquareIndex->GetCount());
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		QCat::Renderer::Submit(m_Shader, m_IndexBuffer->GetCount());
		D3D11_VIEWPORT vp;
		vp.Width =1280;
		vp.Height = 720;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pGfx->GetContext()->RSSetViewports(1u, &vp);
#elif defined(QCAT_OPENGL)
		QCat::Renderer::Submit(m_BlueShader, m_SquareVA);
		QCat::Renderer::Submit(m_Shader, m_VertexArray);
#endif
		QCat::Renderer::EndScene();
	}
	void OnEvent(QCat::Event& event) override
	{
		//QCat::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<QCat::KeyPressedEvent>(BIND_EVENT_FN(ExamLayer::OnKeyPress));
	}

private:
	std::shared_ptr<QCat::Shader> m_Shader;
	std::shared_ptr<QCat::VertexArray> m_VertexArray;

	std::shared_ptr<QCat::Shader> m_BlueShader;
	std::shared_ptr<QCat::VertexArray> m_SquareVA;

	//DX11
	std::unique_ptr<QCat::VertexBuffer> m_VertexBuffer;
	std::unique_ptr<QCat::IndexBuffer> m_IndexBuffer;

	std::unique_ptr<QCat::VertexBuffer> m_SquareBuffer;
	std::unique_ptr<QCat::IndexBuffer> m_SquareIndex;

	//dx11 buffer
	std::unique_ptr<QCat::DX11VertexConstantBuffer> m_constantBuffer;

	//camera
	QCat::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRoatation = 0.0f;
	float m_CameraRoatationSpeed = 0.1f;
};

class Sandbox : public QCat::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExamLayer());
	}
	~Sandbox()
	{
	}
};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}