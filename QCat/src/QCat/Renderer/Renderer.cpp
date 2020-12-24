#include "qcpch.h"
#include "Renderer.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"

#include "Renderer2D.h"
namespace QCat
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::OnWindowResize(unsigned int width, unsigned int height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const Ref<Shader>& shader,const Ref<VertexArray>& vertexArray,const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::Submit(const Ref<Shader>& shader,const unsigned int indexCount, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<DX11Shader>(shader)->UpdateVertexConstantBuffer("u_ViewProjection", &m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<DX11Shader>(shader)->UpdateVertexConstantBuffer("u_Transform", &transform);

		RenderCommand::DrawIndexed(indexCount);
	}
}
