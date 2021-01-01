#pragma once
#include "QCat/Renderer/RenderAPI.h"


namespace QCat
{
	class QGfxDeviceDX11;
	class DX11RenderAPI : public RenderAPI
	{
	public:
		DX11RenderAPI();
		~DX11RenderAPI();
	public:
		virtual void Init() override;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const unsigned int indexCount);
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0) override;
	private:
		QGfxDeviceDX11* pgfx;
		glm::vec4 color;
	};
}