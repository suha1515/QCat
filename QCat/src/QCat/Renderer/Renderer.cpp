#include "qcpch.h"
#include "Renderer.h"

#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"

#include "Renderer2D.h"
namespace QCat
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	struct RendererData
	{
		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<Shader>		 Shader;

	};

	void Renderer::Init()
	{
		QCAT_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
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
		shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix, ShaderType::VS);
		shader->SetMat4("u_Transform", transform, ShaderType::VS);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::Submit(const Ref<Shader>& shader,const unsigned int indexCount, const glm::mat4& transform)
	{
		shader->Bind();
		//std::dynamic_pointer_cast<DXShader>(shader)->UpdateVertexConstantBuffer("u_ViewProjection", &m_SceneData->ViewProjectionMatrix);
		//std::dynamic_pointer_cast<DXShader>(shader)->UpdateVertexConstantBuffer("u_Transform", &transform);

		//RenderCommand::DrawIndexed(indexCount);
	}
}
