#include "qcpch.h"
#include "DX11_RenderAPI.h"
#include "API/DirectX11/QGfxDeviceDX11.h"

namespace QCat
{
	DX11RenderAPI::DX11RenderAPI()
		:color(glm::vec4(0.0f))
	{
		pgfx = nullptr;
	}
	DX11RenderAPI::~DX11RenderAPI()
	{
	}
	void DX11RenderAPI::Init()
	{
		QGfxDeviceDX11* gfx = QGfxDeviceDX11::GetInstance();
		gfx->BlendStateEnable(true);
	}
	void DX11RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		QGfxDeviceDX11* gfx = QGfxDeviceDX11::GetInstance();
		QCAT_CORE_ASSERT(gfx != nullptr, "cast Error!");

		gfx->CleanRenderTarget();
		gfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		gfx->CreateRenderTarget();
		gfx->SetViewPort(width, height);
	}
	void DX11RenderAPI::SetClearColor(const glm::vec4& color)
	{
		this->color = color;
	}
	void DX11RenderAPI::Clear()
	{
		if(pgfx==nullptr)
			pgfx = QGfxDeviceDX11::GetInstance();
		pgfx->BeginFrame(color);
	}
	void DX11RenderAPI::DrawIndexed(const unsigned int indexCount)
	{
		if (pgfx == nullptr)
			pgfx = QGfxDeviceDX11::GetInstance();
		if (pgfx->GetBlendState())
		{
			pgfx->BindBlendState();
		}
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
	void DX11RenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		DrawIndexed(vertexArray->GetIndexBuffer()->GetCount());
	}
}