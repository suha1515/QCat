#include "qcpch.h"
#include "DX11_FrameBuffer.h"
#include "QCat/Renderer/RenderCommand.h"

namespace QCat
{
	namespace Utils {
		static DXGI_FORMAT GetDataType(FramebufferTextureDataFormat format)
		{
			switch (format)
			{
				//32 bit
			case FramebufferTextureDataFormat::RGBA8:			return DXGI_FORMAT_R8G8B8A8_UNORM;
			case FramebufferTextureDataFormat::RED32_INTEGER:   return DXGI_FORMAT_R32_SINT;
				//24 bit
			case FramebufferTextureDataFormat::RGB8:			return DXGI_FORMAT_R8G8B8A8_UNORM;

			}
		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Depth: return true;
			}
			return false;
		}

		static D3D11_TEXTURE2D_DESC CreateTexture2Desc(uint32_t width,uint32_t height,int mipLevels,int arraySize,DXGI_FORMAT format,int samplesCount,int samplesQuality, D3D11_USAGE usage,uint32_t bindFlag,uint32_t cpuAccessFlag=0,uint32_t miscFlags=0)
		{
			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = mipLevels;
			desc.ArraySize = arraySize;
			desc.Format = format;
			desc.SampleDesc.Count = samplesCount;
			desc.SampleDesc.Quality = samplesQuality;
			desc.Usage = usage;
			desc.BindFlags = bindFlag;
			desc.CPUAccessFlags = cpuAccessFlag;
			desc.MiscFlags = miscFlags;

			return desc;
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
				case FramebufferTextureFormat::Texture2D:
				{
					D3D11_TEXTURE2D_DESC texdesc = Utils::CreateTexture2Desc(m_Specification.Width, m_Specification.Height, 1, 1,
						DXGI_FORMAT_R8G8B8A8_UNORM, m_Specification.Samples, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);

					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = texdesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

					m_ColorAttachments[i].textures.push_back(CreateRef<DX11Texture2D>(texdesc));
					m_ColorAttachments[i].rendertargets.push_back(DX11RenderTarget::Create(((DX11Texture2D*)m_ColorAttachments[i].textures[0].get())->GetDXTexture(), rtvDesc));
				}
				break;
				case FramebufferTextureFormat::CubeMap:
				{
					D3D11_TEXTURE2D_DESC texdesc = Utils::CreateTexture2Desc(m_Specification.Width, m_Specification.Height, 1, 6,
						DXGI_FORMAT_R8G8B8A8_UNORM, m_Specification.Samples, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0,
						D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE);

					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = texdesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.ArraySize = 1;
					rtvDesc.Texture2DArray.MipSlice = 0;

					m_ColorAttachments[i].textures.push_back(CreateRef<DX11TextureCube>(texdesc));
					m_ColorAttachments[i].rendertargets.resize(6);
					for (int j= 0; j < 6; ++j)
					{
						rtvDesc.Texture2DArray.FirstArraySlice = j;
						m_ColorAttachments[i].rendertargets[j] = DX11RenderTarget::Create(((DX11TextureCube*)m_ColorAttachments[i].textures[0].get())->GetDXTexture(), rtvDesc);
					}
				}
				break;
				}
			}
		}
		if (m_DepthAttacmentSpecifications.TextureFormat != FramebufferTextureFormat::None)
		{
			switch (m_DepthAttacmentSpecifications.TextureFormat)
			{
			case FramebufferTextureFormat::Depth:
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
			m_RenderTargets.push_back(rendertarget.rendertargets[rendertarget.attachTarget]->GetRenderTargetView());
		}
		if(m_DepthAttachment)
		 gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], m_DepthAttachment->GetDepthStencil());
		else
		 gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], nullptr);
		// only for depth buffer
		if (m_ColorAttachments.empty() && m_DepthAttachment)
			m_DepthAttachment->Bind(gfx);
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
	int DX11FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y,int z)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
		int pixelData;
		if (m_ColorAttachmentSpecifications[attachmentIndex].TextureFormat == FramebufferTextureFormat::CubeMap)
		{
			
			((DX11TextureCube*)m_ColorAttachments[attachmentIndex].textures[0].get())->ReadData(z,x, y, &pixelData);

		}
		else
		{
			((DX11Texture2D*)m_ColorAttachments[attachmentIndex].textures[0].get())->ReadData(x, y, &pixelData);

		}

		return pixelData;
	}
	void DX11FrameBuffer::AttachCubeMapByIndex(uint32_t faceindex)
	{
		for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
		{
			if (m_ColorAttachmentSpecifications[i].TextureFormat == FramebufferTextureFormat::CubeMap)
			{
				m_ColorAttachments[i].attachTarget = faceindex;
			}
		}
		Bind();
	}
	void DX11FrameBuffer::ClearAttachment(uint32_t attachmentIndex, const void* value)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		float r = *(float*)value;
		glm::vec4 color = { r, r, r, r };
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		for(int i=0;i< m_ColorAttachments[attachmentIndex].rendertargets.size();++i)
			m_ColorAttachments[attachmentIndex].rendertargets[i]->Clear(gfx,color);

	}
	void DX11FrameBuffer::SaveColorBuffer(uint32_t index) const
	{
	
	}
	void DX11FrameBuffer::BindColorTexture(uint32_t slot, uint32_t index) const
	{

		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		ID3D11ShaderResourceView* srv= (ID3D11ShaderResourceView*)(m_ColorAttachments[index].textures[0]->GetTexture());

		gfx.GetContext()->PSSetShaderResources(slot, 1u, &srv);
	}
	void DX11FrameBuffer::UnBindTexture()
	{
		ID3D11ShaderResourceView* pSrv = nullptr;
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		gfx.GetContext()->PSSetShaderResources(0, 1u, &pSrv);
	}
	void DX11FrameBuffer::Clear(glm::vec4 color) const
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		for (auto& rendertarget : m_ColorAttachments)
		{
			for (int i = 0; i < rendertarget.rendertargets.size(); ++i)
			{
				rendertarget.rendertargets[rendertarget.attachTarget]->Clear(gfx, { 0.1f,0.1f,0.1f,1.0f });
			}
		}
		if(m_DepthAttachment)
			m_DepthAttachment->Clear(gfx);
	}
}