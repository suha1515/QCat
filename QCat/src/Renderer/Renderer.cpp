#include "qcpch.h"
#include "Renderer.h"

namespace QCat
{
	void Renderer::BeginScene()
	{
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::Submit(const unsigned int indexCount)
	{
		RenderCommand::DrawIndexed(indexCount);
	}
}