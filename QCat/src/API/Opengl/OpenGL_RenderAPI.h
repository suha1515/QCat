#pragma once
#include "QCat/Renderer/RenderAPI.h"

namespace QCat
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;
		virtual void SetDefaultFrameBuffer() override {};
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, unsigned int indexCount=0) override;
	};
}