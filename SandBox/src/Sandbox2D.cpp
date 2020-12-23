#include "Sandbox2D.h"

#include "imgui.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Blender.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
#if defined(QCAT_DX11)
	std::string vertexSrc = "..\\bin\\Debug-windows-\\SandBox\\flatcolor_VS.cso";
	std::string fragmentSrc = "..\\bin\\Debug-windows-\\SandBox\\flatcolor_PS.cso";
	QCat::Ref<QCat::Shader>& vertexShader = QCat::Shader::Create(vertexSrc);
	QCat::Ref<QCat::Shader>& pixelShader = QCat::Shader::Create(fragmentSrc);
	m_FlatColorShader = std::make_shared<QCat::DX11Shader>("Basi", vertexShader, pixelShader);

	QCat::BufferLayout* buflayout = QCat::BufferLayout::Create(
		{ { QCat::ShaderDataType::Float3, "Position" } },
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->GetVerexData().data(),
		std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->GetVerexData().size()
	);

	float squareVertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f
};
	unsigned int stride = sizeof(float) * 3;
	unsigned squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	

	m_VertexBuffer.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices), &stride));
	m_IndexBuffer.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices)));
	m_VertexBuffer->SetLayout(buflayout);
	

	QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
	QCat::Ref<QCat::VertexConstantBuffer> m_constantBuffer;
	QCat::Ref<QCat::VertexConstantBuffer> m_transform;
	QCat::Ref<QCat::PixelConstantBuffer> m_color;

	const glm::mat4& matrix = m_CameraController.GetCamera().GetViewProjectionMatrix();
	glm::mat4 identitymat = glm::mat4(1.0f);
	m_transform.reset(new QCat::VertexConstantBuffer(*pGfx, 1u, &identitymat, sizeof(identitymat)));
	m_color.reset(new QCat::PixelConstantBuffer(*pGfx, 0u, &m_SquareColor, sizeof(m_SquareColor)));
	m_constantBuffer.reset(new QCat::VertexConstantBuffer(*pGfx, 0u, &matrix, sizeof(matrix)));

	std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddVertexConstantBuffer("u_ViewProjection", m_constantBuffer);
	std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddVertexConstantBuffer("u_Transform", m_transform);
	std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->AddPixelConstantBuffer("u_Color", m_color);

#elif defined(QCAT_OPENGL)
	m_SquareVA = QCat::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	QCat::Ref<QCat::VertexBuffer> squareVB;
	squareVB.reset(QCat::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout(QCat::BufferLayout::Create({ 
		{ QCat::ShaderDataType::Float3, "a_Position" } 
		}));
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	QCat::Ref<QCat::IndexBuffer> squareIB;
	squareIB.reset(QCat::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = QCat::Shader::Create("Asset/shaders/glsl/FlatColor.glsl");
#endif
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(QCat::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	QCat::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	QCat::RenderCommand::Clear();

	QCat::Renderer::BeginScene(m_CameraController.GetCamera());
#if defined(QCAT_DX11)
	QCat::QGfxDeviceDX11* pGfx = QCat::QGfxDeviceDX11::GetInstance();
	pGfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_VertexBuffer->Bind();
	m_IndexBuffer->Bind();

	std::dynamic_pointer_cast<QCat::DX11Shader>(m_FlatColorShader)->UpdatePixelConstantBuffer("u_Color", &m_SquareColor);
	QCat::Renderer::Submit(m_FlatColorShader,m_IndexBuffer->GetCount(), glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

#elif defined(QCAT_OPENGL)
	std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<QCat::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	QCat::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

#endif
	
	QCat::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(QCat::Event& e)
{
	m_CameraController.OnEvent(e);
}
