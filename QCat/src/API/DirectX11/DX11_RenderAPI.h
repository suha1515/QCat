#pragma once
#include "Renderer/RenderAPI.h"
#include "QGfxDeviceDX11.h"
namespace QCat
{
	class DX11RenderAPI : public RenderAPI
	{
	public:
		DX11RenderAPI();
		~DX11RenderAPI();
	public:
		virtual void SetClearColor(const DirectX::XMFLOAT4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const unsigned int indexCount) override;
	private:
		QGfxDeviceDX11* pgfx;
		DirectX::XMFLOAT4 color;
	};
}