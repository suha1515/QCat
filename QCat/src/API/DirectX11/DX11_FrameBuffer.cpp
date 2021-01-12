#include "qcpch.h"
#include "DX11_FrameBuffer.h"
#include "QCat/Renderer/RenderCommand.h"

namespace QCat
{
	DX11FrameBuffer::DX11FrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		Invalidate();
	}
	DX11FrameBuffer::~DX11FrameBuffer()
	{
	}
	void DX11FrameBuffer::Invalidate()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		m_renderTarget = CreateRef<DX11RenderTarget>(gfx, m_Specification.Width, m_Specification.Height);
		m_depthStencil = CreateRef<DX11DepthStencil>(gfx, m_Specification.Width, m_Specification.Height);

	}
	void DX11FrameBuffer::Bind()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		m_renderTarget->Bind(gfx,m_depthStencil->GetDepthStencil());
		m_renderTarget->Clear(gfx, { 0.1f,0.1f,0.1f,1.0f });
		m_depthStencil->Clear(gfx);

		// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = (float)m_Specification.Width;
		vp.Height = (float)m_Specification.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		gfx.GetContext()->RSSetViewports(1u, &vp);
	}
	void DX11FrameBuffer::UnBind()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		m_renderTarget->UnBind(gfx);
	}
	void DX11FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		m_Specification.Width = width;
		m_Specification.Height = height;
		m_renderTarget->Resize(gfx,width, height);
		m_depthStencil->Resize(gfx,width, height);
	}
}