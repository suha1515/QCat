#include <QCat.h>
#include "imgui.h"
#include "API/Opengl/OpenGLShader.h"
class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example"),m_Camera(-1.6f,1.6f,-0.9f,0.9f)
	{
		m_CameraPosition = glm::vec3(0.0f);
		std::string vertexSrc;
		std::string fragmentSrc;
		std::string flatColorShaderVertexSrc;
		std::string flatColorShaderFragmentSrc;
#if defined(QCAT_DX11)
		vertexSrc = "..\\bin\\Debug-windows-\\QCat\\Solid_VS.cso";
		fragmentSrc = "..\\bin\\Debug-windows-\\QCat\\Solid_PS.cso";
		m_Shader.reset(QCat::Shader::Create(vertexSrc, fragmentSrc));

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
			std::dynamic_pointer_cast<QCat::DX11Shader>(m_Shader)->GetVerexData().data(),
			std::dynamic_pointer_cast<QCat::DX11Shader>(m_Shader)->GetVerexData().size()
		);
		m_VertexBuffer->SetLayout(buflayout);

		float squareVertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f
		};
		stride = sizeof(float) * 3;

		unsigned squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareBuffer.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices), &stride));
		m_SquareIndex.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices)));

		flatColorShaderVertexSrc = "..\\bin\\Debug-windows-\\QCat\\Square_VS.cso";
		flatColorShaderFragmentSrc = "..\\bin\\Debug-windows-\\QCat\\Square_PS.cso";
		m_FlatColorShader.reset(QCat::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));


		QCat::BufferLayout* squareLayout = QCat::BufferLayout::Create(
			{ {QCat::ShaderDataType::Float3,"Position"} },
			std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->GetVerexData().data(),
			std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->GetVerexData().size()
		);

		m_SquareBuffer->SetLayout(squareLayout);

		const glm::mat4& matrix = m_Camera.GetViewProjectionMatrix();
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		m_constantBuffer.reset(new QCat::VertexConstantBuffer(*pGfx, 0u, &matrix, sizeof(matrix)));
		glm::mat4 identitymat = glm::mat4(1.0f);
		m_transform.reset(new QCat::VertexConstantBuffer(*pGfx, 1u, &identitymat, sizeof(identitymat)));

		m_color.reset(new QCat::PixelConstantBuffer(*pGfx, 0u, &m_SquareColor, sizeof(m_SquareColor)));


		std::dynamic_pointer_cast<QCat::DX11Shader>(m_Shader)->AddVertexConstantBuffer("u_ViewProjection", m_constantBuffer);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddVertexConstantBuffer("u_ViewProjection", m_constantBuffer);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddVertexConstantBuffer("u_Transform", m_transform);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddPixelConstantBuffer("u_Color", m_color);

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
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<QCat::VertexBuffer> squareVB;
		squareVB.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(QCat::BufferLayout::Create({ { QCat::ShaderDataType::Float3, "a_Position" } }));
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<QCat::IndexBuffer> squareIB;
		squareIB.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		vertexSrc = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Color;

				uniform mat4 u_ViewProjection;
				uniform mat4 u_Transform;

				out vec3 v_Position;
				out vec4 v_Color;	

				void main()
				{
					v_Position = a_Position;
					v_Color = a_Color;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
				}
			)";

		 fragmentSrc = R"(
				#version 330 core

				layout(location = 0) out vec4 color;

				in vec3 v_Position;
				in vec4 v_Color;

				void main()
				{
					color = v_Color;
				}
			)";
		m_Shader.reset(QCat::Shader::Create(vertexSrc, fragmentSrc));

		flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
	
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform *  vec4(a_Position, 1.0);	
			}
		)";

		flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader.reset(QCat::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));
#endif	
	}
	void OnUpdate(QCat::Timestep step) override
	{
		//QCAT_TRACE("Deltime: {0}s ({1}ms),", step.GetSeconds(), step.GetMilliseconds());

		if (QCat::Input::IsKeyPressed(VK_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * step;
		else if (QCat::Input::IsKeyPressed(VK_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * step;

		if (QCat::Input::IsKeyPressed(VK_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * step;
		else if (QCat::Input::IsKeyPressed(VK_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * step;

		if (QCat::Input::IsKeyPressed('A'))
			m_CameraRoatation += m_CameraRoatationSpeed * step;
		if (QCat::Input::IsKeyPressed('D'))
			m_CameraRoatation -= m_CameraRoatationSpeed * step;

		QCat::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1 });
		QCat::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRoatation);

		QCat::Renderer::BeginScene(m_Camera);
#ifdef QCAT_DX11
		QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
		pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//m_constantBuffer->UpdateData(*pGfx, &m_Camera.GetViewProjectionMatrix());
		//m_constantBuffer->Bind(*pGfx);
		m_SquareBuffer->Bind();
		m_SquareIndex->Bind();
		//m_transform->Bind(*pGfx);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->UpdatePixelConstantBuffer("u_Color", &m_SquareColor);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(0.11f*x, 0.11f*y, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				QCat::Renderer::Submit(m_FlatColorShader, m_SquareIndex->GetCount(),transform);
			}
		}

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

	
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0;x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y*0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				QCat::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}
		
		QCat::Renderer::Submit(m_Shader, m_VertexArray);
#endif
		QCat::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	void OnEvent(QCat::Event& event) override
	{
		//QCat::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<QCat::KeyPressedEvent>(BIND_EVENT_FN(ExamLayer::OnKeyPress));
	}

private:
	std::shared_ptr<QCat::Shader> m_Shader;
	std::shared_ptr<QCat::VertexArray> m_VertexArray;

	std::shared_ptr<QCat::Shader> m_FlatColorShader;
	std::shared_ptr<QCat::VertexArray> m_SquareVA;

	//DX11
	std::unique_ptr<QCat::VertexBuffer> m_VertexBuffer;
	std::unique_ptr<QCat::IndexBuffer> m_IndexBuffer;

	std::unique_ptr<QCat::VertexBuffer> m_SquareBuffer;
	std::unique_ptr<QCat::IndexBuffer> m_SquareIndex;

	//dx11 buffer
	std::shared_ptr<QCat::VertexConstantBuffer> m_constantBuffer;
	std::shared_ptr<QCat::VertexConstantBuffer> m_transform;
	std::shared_ptr<QCat::PixelConstantBuffer> m_color;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	//camera
	QCat::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.0f;
	float m_CameraRoatation = 0.0f;
	float m_CameraRoatationSpeed = 180;

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