#include "qcpch.h"
#include "DX11_FrameBuffer.h"
#include "QCat/Renderer/RenderCommand.h"
#include "API/DirectX11/DXUtils.h"

namespace QCat
{
	namespace Utils {
	
		static bool IsDepthFormat(FramebufferUsage format)
		{
			switch (format)
			{
			case FramebufferUsage::Depth: return true;
			case FramebufferUsage::Depth_Stencil: return true;
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
			if (!Utils::IsDepthFormat(spec.usage))
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
		if (m_DepthAttachment.depthStencil)
		{
			m_DepthAttachment.depthStencil.reset();
		}

		bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
			{
				switch (m_ColorAttachmentSpecifications[i].textureType)
				{
				case TextureType::Texture2D:
				{
					Sampler_Desc desc;
					desc.MIN = Filtering::POINT;
					desc.MIP = Filtering::POINT;
					desc.MAG = Filtering::POINT;
					desc.addressU = WrapingMode::CLAMP;
					desc.addressV = WrapingMode::CLAMP;
					desc.addressW = WrapingMode::CLAMP;

					D3D11_TEXTURE2D_DESC texdesc = Utils::CreateTexture2Desc(m_Specification.Width, m_Specification.Height, 1, 1,
						Utils::GetDirectDataType(m_ColorAttachmentSpecifications[i].textureformat), m_Specification.Samples, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);

					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = texdesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };

					m_ColorAttachments[i].textures.push_back(CreateRef<DX11Texture2D>(texdesc, desc));
					m_ColorAttachments[i].rendertargets.push_back(DX11RenderTarget::Create(((DX11Texture2D*)m_ColorAttachments[i].textures[0].get())->GetDXTexture(), rtvDesc));
				}
				break;
				case TextureType::TextureCube:
				{
					Sampler_Desc desc;
					D3D11_TEXTURE2D_DESC texdesc = Utils::CreateTexture2Desc(m_Specification.Width, m_Specification.Height, 1, 6,
						Utils::GetDirectDataType(m_ColorAttachmentSpecifications[i].textureformat), m_Specification.Samples, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0,
						D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE);

					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = texdesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.ArraySize = 1;
					rtvDesc.Texture2DArray.MipSlice = 0;

					m_ColorAttachments[i].textures.push_back(CreateRef<DX11TextureCube>(texdesc,desc));
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
		if (m_DepthAttacmentSpecifications.textureType!= TextureType::None)
		{
			DX11DepthStencil::Usage usage;
			DXGI_FORMAT textureformat;
			DXGI_FORMAT DepthStencilViewformat;
			DXGI_FORMAT shaderViewFormat;
			if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth)
			{
				usage = DX11DepthStencil::Usage::Depth;
				textureformat = DXGI_FORMAT_R32_TYPELESS;
				DepthStencilViewformat = DXGI_FORMAT_D32_FLOAT;
				shaderViewFormat = DXGI_FORMAT_R32_FLOAT;
			}
			else if (m_DepthAttacmentSpecifications.usage == FramebufferUsage::Depth_Stencil)
			{
				usage = DX11DepthStencil::Usage::DepthStencil;
				textureformat = DXGI_FORMAT_R24G8_TYPELESS;
				DepthStencilViewformat = DXGI_FORMAT_D24_UNORM_S8_UINT;
				shaderViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			}
			auto format = m_DepthAttacmentSpecifications.textureformat;
			D3D11_TEXTURE2D_DESC texDesc = {};
			texDesc.Width = m_Specification.Width;
			texDesc.Height = m_Specification.Height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = Utils::GetDirectDataType(format);
			texDesc.SampleDesc.Count = m_Specification.Samples;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			switch (m_DepthAttacmentSpecifications.textureType)
			{					
			case TextureType::Texture2D:
			{
				Sampler_Desc desc;
				desc.MIN = Filtering::POINT;
				desc.MIP = Filtering::POINT;
				desc.MAG = Filtering::POINT;
				desc.addressU = WrapingMode::CLAMP;
				desc.addressV = WrapingMode::CLAMP;
				desc.addressW = WrapingMode::CLAMP;
				texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
				Ref<DX11Texture2D> texture = CreateRef<DX11Texture2D>(texDesc, desc, m_Specification.Width, m_Specification.Height);
				m_DepthAttachment.texture = texture;
				m_DepthAttachment.depthStencil = CreateRef<DX11DepthStencil>(gfx, texture->GetDXTexture(), usage);
			}
				break;
			case TextureType::TextureCube:
			{
				Sampler_Desc desc;
				texDesc.ArraySize = 6;
				texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
				Ref<DX11TextureCube> texturecube = CreateRef<DX11TextureCube>(texDesc, desc, m_Specification.Width, m_Specification.Height);
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Flags = 0;
				dsvDesc.Format = DepthStencilViewformat;
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.ArraySize = 6;
				dsvDesc.Texture2DArray.MipSlice = 0;
				dsvDesc.Texture2DArray.FirstArraySlice = 0;

				m_DepthAttachment.texture = texturecube;
				m_DepthAttachment.depthStencil = CreateRef<DX11DepthStencil>(gfx,texturecube->GetDXTexture(),dsvDesc,usage);
			}		
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
		if (m_ColorAttachments.size() > 0)
		{
			if(m_DepthAttachment.depthStencil)
				gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], m_DepthAttachment.depthStencil->GetDepthStencil());
			else
				gfx.GetContext()->OMSetRenderTargets(m_RenderTargets.size(), &m_RenderTargets[0], nullptr);
		}
		else
		{
			// only for depth buffer
			if (m_ColorAttachments.empty() && m_DepthAttachment.depthStencil)
				m_DepthAttachment.depthStencil->Bind(gfx);
		}
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

		ID3D11RenderTargetView* nullRTV[8] = { nullptr };
		gfx.GetContext()->OMSetRenderTargets(8, nullRTV, nullptr);

		ID3D11ShaderResourceView* const pSRV[16] = { NULL };
		gfx.GetContext()->PSSetShaderResources(0, 16, pSRV);
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
		if (m_ColorAttachmentSpecifications[attachmentIndex].textureType == TextureType::TextureCube)
		{
			
			((DX11TextureCube*)m_ColorAttachments[attachmentIndex].textures[0].get())->ReadData(z,x, y, &pixelData);

		}
		else
		{
			((DX11Texture2D*)m_ColorAttachments[attachmentIndex].textures[0].get())->ReadData(x, y, &pixelData);

		}

		return pixelData;
	}
	void DX11FrameBuffer::AttachColorBuffer(uint32_t slot, uint32_t index, int faceindex)
	{
		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		ID3D11DepthStencilView* depthView = nullptr;
		ID3D11RenderTargetView* rtv = nullptr;
		if (m_DepthAttachment.depthStencil != nullptr)
		{
			depthView = m_DepthAttachment.depthStencil->GetDepthStencil();
			m_DepthAttachment.depthStencil->Clear(gfx);
		}
		if (m_ColorAttachmentSpecifications[index].textureType == TextureType::TextureCube)
		{
			rtv = m_ColorAttachments[index].rendertargets[faceindex]->GetRenderTargetView();
			gfx.GetContext()->OMSetRenderTargets(1, &rtv, depthView);
		}
		else
		{
			rtv = m_ColorAttachments[index].rendertargets[0]->GetRenderTargetView();
			gfx.GetContext()->OMSetRenderTargets(1, &rtv, depthView);
		}
	}
	void DX11FrameBuffer::AttachCubeMapByIndex(uint32_t faceindex)
	{
		for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
		{
			if (m_ColorAttachmentSpecifications[i].textureType == TextureType::TextureCube)
			{
				m_ColorAttachments[i].attachTarget = faceindex;
			}
		}
		Bind();
	}
	void DX11FrameBuffer::ClearAttachment(uint32_t attachmentIndex, const void* value)
	{
		QCAT_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		float r = *(int*)value;
		glm::vec4 color = { r, r, r, r };
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		int mainindex = m_ColorAttachments[attachmentIndex].attachTarget;
		m_ColorAttachments[attachmentIndex].rendertargets[mainindex]->Clear(gfx,color);

	}
	void DX11FrameBuffer::ClearDepthStencil()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		if (m_DepthAttachment.depthStencil)
			m_DepthAttachment.depthStencil->Clear(gfx);
	}
	void DX11FrameBuffer::SaveColorBuffer(uint32_t index) const
	{
	
	}
	void DX11FrameBuffer::BindColorTexture(uint32_t slot, uint32_t index) const
	{

		QCAT_CORE_ASSERT(index < m_ColorAttachments.size());

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		m_ColorAttachments[index].textures[0]->Bind(slot);
	}
	void DX11FrameBuffer::BindDepthTexture(uint32_t slot) const
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		/*ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)(m_DepthAttachment.texture->GetTexture());

		gfx.GetContext()->PSSetShaderResources(slot, 1u, &srv);*/
		m_DepthAttachment.texture->Bind(slot);
	}
	void DX11FrameBuffer::UnBindTexture()
	{
		ID3D11ShaderResourceView* pSrv = nullptr;
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		
		for (int i = 0; i < 8; ++i)
		{
			gfx.GetContext()->PSSetShaderResources(i, 1u, &pSrv);
		}
	}
	void DX11FrameBuffer::Clear(glm::vec4 color) const
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		for (auto& rendertarget : m_ColorAttachments)
		{
			auto& rtvArray = rendertarget.rendertargets;
			for (auto& rtv : rtvArray)
			{
					rtv->Clear(gfx, color);
			}
		}
		if(m_DepthAttachment.depthStencil)
			m_DepthAttachment.depthStencil->Clear(gfx);
	}
	Ref<Texture> DX11FrameBuffer::GetColorTexture(uint32_t index) const
	{
		return m_ColorAttachments[index].textures[0];
	}
	Ref<Texture> DX11FrameBuffer::GetDepthTexture() const
	{
		return m_DepthAttachment.texture;
	}
	void DX11FrameBuffer::SetViewport(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}
	void DX11FrameBuffer::CopyFrameBuffer(int srcx0, int srcy0, int srcx1, int srcy1, int dstx0, int dsty0, int dstx1, int dsty1, BufferBit bufferbit, void* destBuffer)
	{
		D3D11_BOX box;
		box.left = srcx0;
		box.right = srcx1;
		box.top = srcy0;
		box.bottom = srcy1;
		box.front = 0;
		box.back = 1;
		if (destBuffer == nullptr)
		{
			switch (bufferbit)
			{
			case BufferBit::Color:
			{
				Ref<DX11Texture2D> srcTex = std::static_pointer_cast<DX11Texture2D>(m_ColorAttachments[0].textures[0]);
				QGfxDeviceDX11::GetInstance()->GetContext()->CopySubresourceRegion(QGfxDeviceDX11::GetInstance()->GetBackBuffer(), 0, dstx0, dsty0, 0, srcTex->GetDXTexture(), 0, &box);
			}
			case BufferBit::Depth:
			{	Ref<DX11Texture2D> srcTex = std::static_pointer_cast<DX11Texture2D>(m_DepthAttachment.texture);
				QGfxDeviceDX11::GetInstance()->GetContext()->CopySubresourceRegion(QGfxDeviceDX11::GetInstance()->GetDepthStencilBuffer(), 0, 0, 0, 0, srcTex->GetDXTexture(), 0, NULL);
			}
			}
		}
		else
		{
			DX11FrameBuffer* framebuffer = (DX11FrameBuffer*)destBuffer;

			switch (bufferbit)
			{
			case BufferBit::Color:
			{
				Ref<DX11Texture2D> destTex = std::static_pointer_cast<DX11Texture2D>(framebuffer->m_ColorAttachments[0].textures[0]);
				Ref<DX11Texture2D> srcTex = std::static_pointer_cast<DX11Texture2D>(m_ColorAttachments[0].textures[0]);
				QGfxDeviceDX11::GetInstance()->GetContext()->CopySubresourceRegion(destTex->GetDXTexture(), 0, dstx0, dsty0, 0, srcTex->GetDXTexture(), 0, &box);
			}
			case BufferBit::Depth:
			{
				Ref<DX11Texture2D> destTex = std::static_pointer_cast<DX11Texture2D>(framebuffer->m_DepthAttachment.texture);
				Ref<DX11Texture2D> srcTex = std::static_pointer_cast<DX11Texture2D>(m_DepthAttachment.texture);
				QGfxDeviceDX11::GetInstance()->GetContext()->CopySubresourceRegion(
					destTex->GetDXTexture(), 0, 0, 0, 0, srcTex->GetDXTexture(), 0, NULL);
			}
			case BufferBit::Stencil:break;
			}

		}
	}
}