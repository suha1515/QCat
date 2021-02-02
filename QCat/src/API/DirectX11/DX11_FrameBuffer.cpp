#include "qcpch.h"
#include "DX11_FrameBuffer.h"
#include "QCat/Renderer/RenderCommand.h"

namespace QCat
{
	namespace Utils {
		static void CreateTexture(bool multisampled)
		{

		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}
	}
	DX11FrameBuffer::DX11FrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttacmentSpecifications = spec;
		}
		Invalidate();
	}
	DX11FrameBuffer::~DX11FrameBuffer()
	{
	}
	void DX11FrameBuffer::Invalidate()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		if (!m_ColorAttachments.empty())
		{
			m_ColorAttachments.clear();
		}
		if (m_DepthAttachment)
		{
			m_DepthAttachment.reset();
		}

		bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
			{
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					m_ColorAttachments[i] = CreateRef<DX11RenderTarget>(gfx, m_Specification.Width, m_Specification.Height,m_Specification.Samples, DXGI_FORMAT_R8G8B8A8_UNORM);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					m_ColorAttachments[i] = CreateRef<DX11RenderTarget>(gfx, m_Specification.Width, m_Specification.Height, m_Specification.Samples, DXGI_FORMAT_R32_SINT);
					break;
				}
			}
		}
		if (m_DepthAttacmentSpecifications.TextureFormat != FramebufferTextureFormat::None)
		{
			switch (m_DepthAttacmentSpecifications.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				m_DepthAttachment = CreateRef<DX11DepthStencil>(gfx, m_Specification.Width, m_Specification.Height,m_Specification.Samples, DXGI_FORMAT_D24_UNORM_S8_UINT);
				break;
			}
			
		}	
	}
	void DX11FrameBuffer::Bind()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		std::vector<ID3D11RenderTargetView*> m_RenderTargets;
		for (auto& rendertarget : m_ColorAttachments)
		{
			rendertarget->Clear(gfx, { 0.1f,0.1f,0.1f,1.0f });
			m_RenderTargets.push_back(rendertarget->GetRenderTargetView());
		}
		if(m_DepthAttachment)
		 gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], m_DepthAttachment->GetDepthStencil());
		else
		 gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], nullptr);
		// only for depth buffer
		if (m_ColorAttachments.empty() && m_DepthAttachment)
			m_DepthAttachment->Bind(gfx);

		m_DepthAttachment->Clear(gfx);

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

		gfx.GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
	void DX11FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}
	int DX11FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
		int pixelData;
		m_ColorAttachments[attachmentIndex]->ReadData(x, y,&pixelData);
		return pixelData;
	}
}