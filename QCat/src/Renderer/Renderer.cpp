#include "qcpch.h"
#include "Renderer.h"

namespace QCat
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader,const std::shared_ptr<VertexArray>& vertexArray,const glm::mat4& transform)
	{
		shader->Bind();
		shader->UploadeUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->UploadeUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader,const unsigned int indexCount)
	{
		shader->Bind();
		RenderCommand::DrawIndexed(indexCount);
	}
}
