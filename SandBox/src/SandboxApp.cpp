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

		float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,0.0f,1.0f,
		-0.5f,  0.5f, 0.0f,0.0f,0.0f,
		 0.5f,  0.5f, 0.0f,1.0f,0.0f,
		 0.5f, -0.5f, 0.0f,1.0f,1.0f
		};
		stride = sizeof(float) * 5;

		unsigned squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_SquareBuffer.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices), &stride));
		m_SquareIndex.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices)));

		flatColorShaderVertexSrc = "..\\bin\\Debug-windows-\\QCat\\Square_VS.cso";
		flatColorShaderFragmentSrc = "..\\bin\\Debug-windows-\\QCat\\Square_PS.cso";
		m_FlatColorShader.reset(QCat::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));


		QCat::BufferLayout* squareLayout = QCat::BufferLayout::Create(
			{ {QCat::ShaderDataType::Float3,"Position"},
			  {QCat::ShaderDataType::Float2,"Texcoord"}
			},
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

		

		std::string textureShaderVertexSrc = "..\\bin\\Debug-windows-\\QCat\\flatcolor_VS.cso";
		std::string textureShaderFragmentSrc = "..\\bin\\Debug-windows-\\QCat\\flatcolor_PS.cso";

		m_TextureShader.reset(QCat::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_TextureShader)->AddVertexConstantBuffer("u_ViewProjection", m_constantBuffer);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_TextureShader)->AddVertexConstantBuffer("u_Transform", m_transform);
		//m_Texture = (QCat::Texture2D::Create("Asset/textures/ranka.jpg"));
		m_Texture = (QCat::Texture2D::Create("Asset/textures/Checkerboard.png"));
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		pGfx->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler);

#elif defined(QCAT_OPENGL)
		m_VertexArray.reset(QCat::VertexArray::Create());

		float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		QCat::Ref<QCat::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(QCat::VertexBuffer::Create(vertices, sizeof(vertices)));

		QCat::BufferLayout* buflayout = QCat::BufferLayout::Create(
			{ { QCat::ShaderDataType::Float3, "a_Position" },
				{ QCat::ShaderDataType::Float4, "a_Color" } }
		);
		vertexBuffer->SetLayout(buflayout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0,1,2 };
		QCat::Ref<QCat::IndexBuffer> indexbuffer;
		indexbuffer.reset(QCat::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_VertexArray->SetIndexBuffer(indexbuffer);


		m_SquareVA.reset(QCat::VertexArray::Create());
		float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,0.0f,0.0f,
		 0.5f, -0.5f, 0.0f,1.0f,0.0f,
		 0.5f,  0.5f, 0.0f,1.0f,1.0f,
		-0.5f,  0.5f, 0.0f,0.0f,1.0f
		};

		QCat::Ref<QCat::VertexBuffer> squareVB;
		squareVB.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(QCat::BufferLayout::Create({ 
			{ QCat::ShaderDataType::Float3, "a_Position" },
			{ QCat::ShaderDataType::Float2, "a_TexCoord" }
			}));
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		QCat::Ref<QCat::IndexBuffer> squareIB;
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

		std::string textureShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
	
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform *  vec4(a_Position, 1.0);	
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture,v_TexCoord);
			}
		)";

		m_TextureShader.reset(QCat::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));
		m_Texture = (QCat::Texture2D::Create("Asset/textures/Checkerboard.png"));

		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
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
		pGfx->GetContext()->PSSetSamplers(0, 1, pSampler.GetAddressOf());
		//m_constantBuffer->UpdateData(*pGfx, &m_Camera.GetViewProjectionMatrix());
		//m_constantBuffer->Bind(*pGfx);
		m_SquareBuffer->Bind();
		m_SquareIndex->Bind();
		//m_transform->Bind(*pGfx);
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->UpdatePixelConstantBuffer("u_Color", &m_SquareColor);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		/*for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(0.11f*x, 0.11f*y, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				QCat::Renderer::Submit(m_FlatColorShader, m_SquareIndex->GetCount(),transform);
			}
		}*/
		m_Texture->Bind();
		QCat::Renderer::Submit(m_TextureShader, m_SquareIndex->GetCount(), glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		QCat::Renderer::Submit(m_Shader, m_SquareIndex->GetCount());
		D3D11_VIEWPORT vp;
		vp.Width =1280;
		vp.Height = 720;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pGfx->GetContext()->RSSetViewports(1u, &vp);
#elif defined(QCAT_OPENGL)

		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0;x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y*0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				QCat::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}
		m_Texture->Bind();
		QCat::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		// QCat::Renderer::Submit(m_Shader, m_VertexArray);
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
	QCat::Ref<QCat::Shader> m_Shader;
	QCat::Ref<QCat::VertexArray> m_VertexArray;

	QCat::Ref<QCat::Shader> m_FlatColorShader, m_TextureShader;
	QCat::Ref<QCat::VertexArray> m_SquareVA;

	QCat::Ref<QCat::Texture2D> m_Texture;

	//DX11
	QCat::Ref<QCat::VertexBuffer> m_VertexBuffer;
	QCat::Ref<QCat::IndexBuffer> m_IndexBuffer;

	QCat::Ref<QCat::VertexBuffer> m_SquareBuffer;
	QCat::Ref<QCat::IndexBuffer> m_SquareIndex;

	//dx11 buffer
	QCat::Ref<QCat::VertexConstantBuffer> m_constantBuffer;
	QCat::Ref<QCat::VertexConstantBuffer> m_transform;
	QCat::Ref<QCat::PixelConstantBuffer> m_color;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
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