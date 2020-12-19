#include "qcpch.h"
#include "DX11_RenderAPI.h"

namespace QCat
{
	DX11RenderAPI::DX11RenderAPI()
	{
		pgfx = nullptr;
	}
	DX11RenderAPI::~DX11RenderAPI()
	{
	}
	void DX11RenderAPI::Init()
	{
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
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
}