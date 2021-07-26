#include "qcpch.h"
#include "DX11_FrameBufferEx.h"
#include "QCat/Renderer/RenderCommand.h"
#include "DXUtils.h"

namespace QCat
{
	DirectXFrameBufferEx::DirectXFrameBufferEx(const AttachmentSpecification& attachments)
		:m_AttachmentSpecifications(attachments.Attachments),m_spec(attachments)
	{
		uint32_t ColorBufferCount = 0;
		uint32_t DepthBufferCount = 0;
		for (auto spec : m_AttachmentSpecifications)
		{
			if (spec.usage != FramebufferUsage::Depth && spec.usage != FramebufferUsage::Depth_Stencil)
			{
				ColorBufferCount++;
				colorbit = true;
			}
			else if (spec.usage == FramebufferUsage::Depth)
			{
				DepthBufferCount++;
				depthbit = true;
			}
			else if (spec.usage == FramebufferUsage::Depth_Stencil)
			{
				DepthBufferCount++;
				stencilbit = true;
				depthbit = true;
			}
		}
		m_RenderTargets.resize(8);
		m_ColorAttachments.resize(8);
	}
	DirectXFrameBufferEx::~DirectXFrameBufferEx()
	{
	}
	void DirectXFrameBufferEx::Bind()
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		ID3D11RenderTargetView* nullRTV[8] = { nullptr };
		gfx.GetContext()->OMSetRenderTargets(8, nullRTV, nullptr);

		ID3D11ShaderResourceView* const pSRV[16] = { NULL };
		gfx.GetContext()->PSSetShaderResources(0, 16, pSRV);

		std::vector<ID3D11RenderTargetView*> renderTargets;
		/*for (auto& renderTarget : m_ColorAttachments)
		{
			if (renderTarget)
				renderTargets.push_back(renderTarget->GetRenderTargetView());
			else
				renderTargets.push_back(nullptr);
		}
		if (m_DepthAttachment)
			gfx.GetContext()->OMSetRenderTargets(renderTargets.size(), &renderTargets[0], m_DepthAttachment->GetDepthStencil());
		else
			gfx.GetContext()->OMSetRenderTargets(renderTargets.size(), &renderTargets[0], nullptr);*/

		for (auto& renderTarget : m_RenderTargets)
		{
			if (renderTarget)
				renderTargets.push_back((ID3D11RenderTargetView*)renderTarget->GetTextureView());
			else
				renderTargets.push_back(nullptr);
		}
		if (m_DepthStencilView)
			gfx.GetContext()->OMSetRenderTargets(renderTargets.size(), &renderTargets[0], (ID3D11DepthStencilView *)m_DepthStencilView->GetTextureView());
		else
			gfx.GetContext()->OMSetRenderTargets(renderTargets.size(), &renderTargets[0], nullptr); 

	}
	void DirectXFrameBufferEx::UnBind()
	{
		RenderCommand::SetDefaultFrameBuffer();
	}
	void DirectXFrameBufferEx::InitializeTexture(const std::string& name, const Texture_Desc& texDesc, const Sampler_Desc& samplerDesc)
	{
		TextureFormat format = TextureFormat::None;
		TextureType type = TextureType::None;
		FramebufferUsage usage;

		for (auto& spec : m_AttachmentSpecifications)
		{
			if (spec.name == name)
			{
				format = spec.textureformat;
				type = spec.textureType;
				usage = spec.usage;
				break;
			}
		}
		if (format == TextureFormat::None)
		{
			QCAT_CORE_ERROR("FrameBuffer Init Texture Error : there are no specification for name ");
			//QCAT_CORE_ASSERT(false)
			//std::runtime_error("FrameBuffer Init Texture Error : there are no specification for name");
		}
		Ref<Texture> texture;
		switch (type)
		{
		case TextureType::None:
			QCAT_CORE_ERROR("FrameBuffer Init Texture Error : There are nono TextureType info");
			break;
		case TextureType::Texture2D:
			texture = Texture2D::Create(format, samplerDesc, texDesc.Width, texDesc.Height, texDesc.MipLevels, texDesc.SampleCount);
			break;
		case TextureType::Texture2DArray:
			break;
		case TextureType::TextureCube:
			texture = TextureCube::Create(format, samplerDesc, texDesc.Width, texDesc.Height, texDesc.MipLevels);
			break;
		}

		auto find_iter = m_Textures.find(name);
		if (find_iter != m_Textures.end())
			m_Textures[name] = texture;
		else
			m_Textures.emplace(name, texture);

	}
	namespace Utils
	{
		uint32_t GetAttachmentIndex(AttachmentType type)
		{
			switch (type)
			{
			case AttachmentType::Color_0:	return 0;
			case AttachmentType::Color_1:	return 1;
			case AttachmentType::Color_2:	return 2;
			case AttachmentType::Color_3:	return 3;
			case AttachmentType::Color_4:	return 4;
			case AttachmentType::Color_5:	return 5;
			case AttachmentType::Color_6:	return 6;
			case AttachmentType::Color_7:	return 7;
			default:
				return 0;
			}
		}
		D3D11_RENDER_TARGET_VIEW_DESC SetRenderTargetInfo(TextureFormat format, TextureType type,uint32_t arraySize, uint32_t mipLevel,bool multisampled)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = Utils::GetDirectDataType(format);
			switch (type)
			{
			case TextureType::Texture2D:
			{
				rtvDesc.ViewDimension = multisampled ? D3D11_RTV_DIMENSION_TEXTURE2DMS: D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D = D3D11_TEX2D_RTV{ 0 };
				rtvDesc.Texture2D.MipSlice =multisampled ? 0 : mipLevel;
			}
			break;
			case TextureType::Texture2DArray:
			{
				rtvDesc.ViewDimension = multisampled ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				if (multisampled)
				{
					rtvDesc.Texture2DMSArray.ArraySize = arraySize;
					rtvDesc.Texture2DMSArray.FirstArraySlice = 0;
				}
				else
				rtvDesc.Texture2DArray.ArraySize = arraySize;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.MipSlice = mipLevel;
			}
			break;
			case TextureType::TextureCube:
			{
			}
			break;
			}
			return rtvDesc;
		}
		D3D11_DEPTH_STENCIL_VIEW_DESC SetDepthStencilInfo(TextureFormat format, TextureType type, uint32_t arraySize, uint32_t mipLevel, bool multisampled)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Flags = 0;
			DXGI_FORMAT texFormat = Utils::GetDirectDataType(format);
			switch (type)
			{
			case TextureType::Texture2D:
			{
				dsvDesc.Format = Utils::MapTypeDSV(texFormat);

				dsvDesc.ViewDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMS:D3D11_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = multisampled ? 0 : mipLevel;
			}
			break;
			case TextureType::Texture2DArray:
			{
				dsvDesc.Format = Utils::MapTypeDSV(texFormat);
				dsvDesc.ViewDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY: D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				if (multisampled)
				{
					dsvDesc.Texture2DMSArray.ArraySize = arraySize;
					dsvDesc.Texture2DMSArray.FirstArraySlice = 0;
				}
				else
				{
					dsvDesc.Texture2DArray.ArraySize = arraySize;
					dsvDesc.Texture2DArray.FirstArraySlice = 0;
					dsvDesc.Texture2DArray.MipSlice = mipLevel;
				}
			}
			break;
			case TextureType::TextureCube:
			{
				dsvDesc.Format = Utils::MapTypeDSV(texFormat);
				dsvDesc.ViewDimension = multisampled ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				if (multisampled)
				{
					dsvDesc.Texture2DMSArray.ArraySize = 6;
					dsvDesc.Texture2DMSArray.FirstArraySlice =0;
				}
				else
				{
					dsvDesc.Texture2DArray.ArraySize = 6;
					dsvDesc.Texture2DArray.FirstArraySlice = 0;
					dsvDesc.Texture2DArray.MipSlice = mipLevel;
				}
			}
			
			break;
			}
			return dsvDesc;
		}
	}
	void DirectXFrameBufferEx::AttachTexture(const std::string& name,AttachmentType attachType, TextureType type, uint32_t mipLevel, uint32_t layerStart, uint32_t layerLevel)
	{
		Ref<Texture> texture = m_Textures[name];
	}
	void DirectXFrameBufferEx::AttachTexture(const Ref<Texture>& texture, AttachmentType attachType, TextureType type, uint32_t mipLevel, uint32_t layerStart, uint32_t layerLevel)
	{
		if (texture != nullptr)
		{
			TextureFormat format = texture->GetTextureFormat();
			TextureType textype = texture->GetTextureType();
			bool multisampled = texture->GetDesc().SampleCount > 1;

			if (attachType == AttachmentType::Depth || attachType == AttachmentType::Stencil || attachType == AttachmentType::Depth_Stencil)
			{
				Ref<DX11DepthStencilView> depthStencilView;
				depthStencilView = CreateRef<DX11DepthStencilView>(type, texture, format, mipLevel, layerStart, layerLevel);
				m_DepthStencilView = depthStencilView;
			}
			else
			{
				Ref<DX11RenderTargetView> renderTargetView;
				renderTargetView = CreateRef<DX11RenderTargetView>(type, texture, format, mipLevel, layerStart, layerLevel);
				m_RenderTargets[Utils::GetAttachmentIndex(attachType)] = renderTargetView;
			}		
		}
		else
		{
			QCAT_CORE_ERROR("texture is nullptr : DX FrameBufferEX Attach Error");
		}
		Bind();
	}
	void DirectXFrameBufferEx::AttachColorTexture(const Ref<RenderTargetView>& textureView, AttachmentType type)
	{
		m_RenderTargets[Utils::GetAttachmentIndex(type)] = dynamic_pointer_cast<DX11RenderTargetView>(textureView);
	}
	void DirectXFrameBufferEx::AttachDepthTexture(const Ref<DepthStencilView>& textureView, AttachmentType type)
	{
		m_DepthStencilView = dynamic_pointer_cast<DX11DepthStencilView>(textureView);
	}


	void DirectXFrameBufferEx::DetachTexture(AttachmentType attachType)
	{
		if (attachType == AttachmentType::Depth || attachType == AttachmentType::Stencil || attachType == AttachmentType::Depth_Stencil)
		{
			m_DepthAttachment=nullptr;
		}
		else
		{
			m_ColorAttachments[Utils::GetAttachmentIndex(attachType)] = nullptr;
		}
		Bind();
	}
	void DirectXFrameBufferEx::DetachAll()
	{
		m_DepthStencilView.reset();
		for (auto& renderTarget : m_RenderTargets)
			renderTarget.reset();
	}
	Ref<Texture> DirectXFrameBufferEx::GetTexture(const std::string& name)
	{
		auto find_iter = m_Textures.find(name);
		if (find_iter != m_Textures.end())
			return find_iter->second;
		else
			return nullptr;
	}
	void DirectXFrameBufferEx::Clear(const glm::vec4 color)
	{
		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();

		for (auto& rendertarget : m_RenderTargets)
		{
			if(rendertarget!=nullptr)
				rendertarget->Clear(color);
		}
		if(m_DepthStencilView)
			m_DepthStencilView->Clear();
	}
}