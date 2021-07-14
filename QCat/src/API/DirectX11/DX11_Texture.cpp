#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
#include "DXUtils.h"
#include "DX11_Sampler.h"
#include "QCat/Uitiliy/Float16.h"
namespace QCat
{
	namespace Utils
	{
		void* LoadImageFromFile(const std::string& path, bool gamacorrection, bool flip, uint32_t& width, uint32_t& height, uint32_t channel, DXGI_FORMAT& format)
		{
			int width_, height_, channels_;
			if (!flip)
				stbi_set_flip_vertically_on_load(0);
			else
				stbi_set_flip_vertically_on_load(1);
			auto begin = path.find_last_of('.');
			std::string extension = path.substr(begin + 1, path.length());
			void* stb_data = nullptr;
			void* pData = nullptr;
			Float16* float16Array = nullptr;
			unsigned char* UData = nullptr;

			if (extension != "hdr")
			{
				stb_data = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load Image!");

			}
			else
			{
				stb_data = stbi_loadf(path.c_str(), &width_, &height_, &channels_, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load HDR image!");

				float16Array = new Float16[width * height * 4];
				for (unsigned int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
				{
					float16Array[i] = ((float*)stb_data)[j];
					float16Array[i + 1] = ((float*)stb_data)[j + 1];
					float16Array[i + 2] = ((float*)stb_data)[j + 2];
					float16Array[i + 3] = 1.0f;
				}
			}

			width = width_;
			height = height_;
			channel = channels_;
			{
				if (extension != "hdr")
				{
					switch (channel)
					{
					case 4:
						format = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0; i < width * height * 4; ++i)
							UData[i] = ((unsigned char*)stb_data)[i];
						break;
					case 3:
						format = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
						{
							UData[i] = ((unsigned char*)stb_data)[j];
							UData[i + 1] = ((unsigned char*)stb_data)[j + 1];
							UData[i + 2] = ((unsigned char*)stb_data)[j + 2];
							UData[i + 3] = 255;
						}
						break;
					case 2:
						format = gamacorrection ? DXGI_FORMAT_R8G8_SNORM : DXGI_FORMAT_R8G8_UNORM;
						UData = new unsigned char[width * height * 4];
						for (int i = 0, j = 0; j < width * height * 2; i += 4, j += 2)
						{
							UData[i] = ((unsigned char*)stb_data)[j];
							UData[i + 1] = ((unsigned char*)stb_data)[j + 1];
							UData[i + 2] = 255;
							UData[i + 3] = 255;
						}
						break;
					case 1:
						UData = new unsigned char[width * height];
						format = DXGI_FORMAT_R8_UNORM;
						for (int i = 0; i < width * height; ++i)
							UData[i] = ((unsigned char*)stb_data)[i];
						break;
					}
					pData = UData;
				}
				else
				{
					switch (channel)
					{
					case 4:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0; i < width * height * 4; i++)
							float16Array[i] = ((float*)stb_data)[i];
						break;
					case 3:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height * 3; i += 4, j += 3)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = ((float*)stb_data)[j + 1];
							float16Array[i + 2] = ((float*)stb_data)[j + 2];
							float16Array[i + 3] = 1.0f;
						}
						break;
					case 2:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height * 2; i += 4, j += 2)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = ((float*)stb_data)[j + 1];
							float16Array[i + 2] = 1.0f;
							float16Array[i + 3] = 1.0f;
						}
						break;
					case 1:
						float16Array = new Float16[width * height * 4];
						for (unsigned int i = 0, j = 0; j < width * height; i += 4, j += 1)
						{
							float16Array[i] = ((float*)stb_data)[j];
							float16Array[i + 1] = 1.0f;
							float16Array[i + 2] = 1.0f;
							float16Array[i + 3] = 1.0f;
						}
						break;


					}
					pData = float16Array;
					format = DXGI_FORMAT_R16G16B16A16_FLOAT;
					//m_dataFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}
			return pData;
		}
	}
	DX11Texture2D::DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gammacorrection )
	{
		QCAT_PROFILE_FUNCTION();
		this->pathes.push_back(path);
		
		// Texture Description
		uint32_t width = 0, height = 0, channels = 0;
		DXGI_FORMAT format;
		void* pData = nullptr;
		pData = Utils::LoadImageFromFile(path, gammacorrection, flip, width, height, channels, format);
		m_dataFormat = format;
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		if (mipLevel !=1 && mipLevel>=0)
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(format);
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Invalidate(pData);
		sampler = DX11Sampler::Create(desc);

		if (pData != nullptr)
		{
			free(pData);
			pData = nullptr;
		}
	}
	DX11Texture2D::DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
	{
		m_dataFormat = Utils::GetDirectDataType(format);

		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS && m_dataFormat != DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		else
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		Invalidate(pData);

		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	void DX11Texture2D::GenerateMipMap()
	{
		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap");
	}
	void DX11Texture2D::SetData(void* data, unsigned int size, uint32_t textureindex )
	{
		QCAT_PROFILE_FUNCTION();

		//unsigned int bpc = m_dataFormat == DXGI_FORMAT_R8G8B8A8_UNORM ? 4 : 3;
		//QCAT_CORE_ASSERT(size == m_width * m_height * bpc, "Data must be entire texture!");
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, size, 0u);
	}
	void DX11Texture2D::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		textureDesc.Width = width;
		textureDesc.Height = height;
		this->desc.Width = width;
		this->desc.Height = height;

		Invalidate(nullptr);
	}
	void DX11Texture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		sampler->Bind(slot);
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	void DX11Texture2D::ReadData(uint32_t x, uint32_t y, const void* outdata)
	{
		namespace wrl = Microsoft::WRL;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
		wrl::ComPtr<ID3D11Resource> pResSource;
		pTextureView->GetResource(&pResSource);
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC desc;
	
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = 1;
		textureDesc.Height = 1;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = this->desc.SampleCount;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;

		//텍스처 생성
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

		// 텍스처 복사
		//gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
		D3D11_BOX srcBox;
		srcBox.left = x;
		srcBox.right = x + 1;
		srcBox.bottom = y + 1;
		srcBox.top = y;
		srcBox.front = 0;
		srcBox.back = 1;
		gfx.GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), 0, &srcBox);

		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx.GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		if (x >= 0 && x <= this->desc.Width && y >= 0 && y <= this->desc.Height)
		{
			*(int*)outdata = *(int*)msr.pData;
		}
		gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	}
	void DX11Texture2D::Invalidate(void * pData)
	{
		if (pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();

		if (pData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = pData;
			subResource.SysMemPitch = textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format);
			subResource.SysMemSlicePitch = 0;
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, &subResource, &pTexture);
		}
		else
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(m_dataFormat);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		if (textureDesc.MipLevels > 0 &&(textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
	}
	DX11TextureCube::DX11TextureCube(const std::vector<std::string>& imgPathes, Sampler_Desc desc, unsigned int mipLevel,bool flip, bool gammaCorrection)
	{
		QCAT_PROFILE_FUNCTION();
		void* pData[6];
		uint32_t width = 0, height = 0, channel = 0;
		DXGI_FORMAT format;
		pData[0] = Utils::LoadImageFromFile(imgPathes[0].c_str(), gammaCorrection, flip, width, height, channel, format);

		for (int i = 1; i < 6; ++i)
		{
			uint32_t width_ = 0, height_ = 0, channel_ = 0;
			DXGI_FORMAT format_;
			pData[i] = Utils::LoadImageFromFile(imgPathes[0].c_str(), gammaCorrection, flip, width_, height_, channel_, format_);
			if (width != width_ || height != height_ || channel != channel_ || format != format_)
			{
				QCAT_CORE_ASSERT(false, "DX11TextureCube image isnt same!");
				break;
			}
		}
		m_dataFormat = format;
		// Texture Description
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE ;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE ;
		textureDesc.CPUAccessFlags = 0;
		if (m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS || m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		else
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = Utils::GetTextureFormatFromDX(format);
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = 1;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;
		// Create Texture and update it
		D3D11_SUBRESOURCE_DATA subResource[6];
		for (int i = 0; i < 6; ++i)
		{
			subResource[i].pSysMem = pData[i];
			subResource[i].SysMemPitch = textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format);
			subResource[i].SysMemSlicePitch = 0;
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc,subResource, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		// free loaded image
		for (int i = 0; i < 6; ++i)
		{
			free(pData[i]);
		}
		sampler = DX11Sampler::Create(desc);
	}
	DX11TextureCube::DX11TextureCube(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, void* pData)
	{
		m_dataFormat = Utils::GetDirectDataType(format);
		// Texture Description
		textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS || m_dataFormat == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
			textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		else
			textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = format;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = 1;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		this->desc.bindFlags = textureDesc.BindFlags;

		// Create Texture and update it
		Invalidate(pData);
		sampler = DX11Sampler::Create(desc);
	}
	void DX11TextureCube::GenerateMipMap()
	{
		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap");
	}
	void DX11TextureCube::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
	}
	void DX11TextureCube::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		textureDesc.Width = width;
		textureDesc.Height = height;
		this->desc.Width = width;
		this->desc.Height = height;
		Invalidate(nullptr);
	}
	void DX11TextureCube::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		sampler->Bind(slot);
	}
	void DX11TextureCube::ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata)
	{
		namespace wrl = Microsoft::WRL;

		QGfxDeviceDX11& gfx = *QGfxDeviceDX11::GetInstance();
		// soruce와 호환가능한 임시 텍스쳐를 만든다 하지만 cpu에서 읽고 쓸 수 있다.
		wrl::ComPtr<ID3D11Resource> pResSource;
		pTextureView->GetResource(&pResSource);
		wrl::ComPtr<ID3D11Texture2D> pTexSource;
		pResSource.As(&pTexSource);
		D3D11_TEXTURE2D_DESC desc;

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = 1;
		textureDesc.Height = 1;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 0;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		textureDesc.MiscFlags = 0;
		wrl::ComPtr<ID3D11Texture2D> pTexTemp;

		//텍스처 생성
		gfx.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexTemp);

		//// 텍스처 복사
		//gfx.GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
		int i = D3D11CalcSubresource(0, face, 1);

		D3D11_BOX srcBox;
		srcBox.left = x;
		srcBox.right = x + 1;
		srcBox.bottom = y + 1;
		srcBox.top = y;
		srcBox.front = 0;
		srcBox.back = 1;
		gfx.GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), i, &srcBox);

		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx.GetContext()->Map(pTexTemp.Get(),0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		if (x >= 0 && x <= this->desc.Width && y >= 0 && y <= this->desc.Height)
		{
			outdata = (msr.pData);
		}
		gfx.GetContext()->Unmap(pTexTemp.Get(), 0);
	}
	void DX11TextureCube::Invalidate(void* pData)
	{
		if(pTexture)
			pTexture.Reset();
		if (pTextureView)
			pTextureView.Reset();


		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		if (pData != nullptr)
		{
			for (int i = 0; i < 6; ++i)
			{
				QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), D3D11CalcSubresource(0, i, textureDesc.MipLevels), nullptr, pData, textureDesc.Width * Utils::GetMemoryPitch(textureDesc.Format), 0);
			}
		}
		
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(textureDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
	}
	DX11ShaderView::DX11ShaderView(TextureType type,Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t viewDimension = Utils::GetDimensionFromType(type);
		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);

		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D_SRV_DIMENSION dstDimension = Utils::GetSRVDimensionFromType(type, multisampled);

		bool pass = true;
		if (viewDimension != srcDimension)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : Texture Dimension isnt Compitable!");
			pass = false;
		}
		if (srcTexDesc.MipLevels < startMip || srcTexDesc.MipLevels < startMip + numMip)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		srvDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		srvDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;
		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layercount = numlayer;
		switch (srcTexDesc.Type)
		{
		case TextureType::Texture1D:
			srvDesc.Texture1D.MipLevels = numMip;
			srvDesc.Texture1D.MostDetailedMip = startMip;
			break;
		case TextureType::Texture1DArray:
			srvDesc.Texture1DArray.MipLevels = numMip;
			srvDesc.Texture1DArray.MostDetailedMip = startMip;
			srvDesc.Texture1DArray.ArraySize = numlayer;
			srvDesc.Texture1DArray.FirstArraySlice = startLayer;
			break;
		case TextureType::Texture2D:
			if (!multisampled)
			{
				srvDesc.Texture2D.MipLevels = numMip;
				srvDesc.Texture2D.MostDetailedMip = startMip;
			}
			break;
		case TextureType::Texture2DArray:
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			if (type == TextureType::Texture2D)
				layercount = 1;
			if (!multisampled)
			{
				srvDesc.Texture2DArray.MipLevels = numMip;
				srvDesc.Texture2DArray.MostDetailedMip = startMip;
				srvDesc.Texture2DArray.ArraySize = layercount;
				srvDesc.Texture2DArray.FirstArraySlice = startLayer;
			}
			else
			{
				srvDesc.Texture2DMSArray.ArraySize = layercount;
				srvDesc.Texture2DMSArray.FirstArraySlice = startLayer;
			}
			break;
		case TextureType::TextureCube:
			if (type == TextureType::Texture2D)
				layercount = 1;
			if (type == TextureType::Texture2D || type == TextureType::Texture2DArray)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				if (!multisampled)
				{
					srvDesc.Texture2DArray.MipLevels = numMip;
					srvDesc.Texture2DArray.MostDetailedMip = startMip;
					srvDesc.Texture2DArray.ArraySize = layercount;
					srvDesc.Texture2DArray.FirstArraySlice = startLayer;
				}
				else
				{
					srvDesc.Texture2DMSArray.ArraySize = layercount;
					srvDesc.Texture2DMSArray.FirstArraySlice = startLayer;
				}
			}
			else
			{
				srvDesc.TextureCube.MipLevels = numMip;
				srvDesc.TextureCube.MostDetailedMip = startMip;
			}
			break;
		case TextureType::TextureCubeArray:
			srvDesc.TextureCubeArray.MipLevels = numMip;
			srvDesc.TextureCubeArray.MostDetailedMip = startMip;
			srvDesc.TextureCubeArray.NumCubes = numlayer;
			srvDesc.TextureCubeArray.First2DArrayFace = startLayer;
			break;
		case TextureType::Texture3D:
			srvDesc.Texture3D.MipLevels = numMip;
			srvDesc.Texture3D.MostDetailedMip = startMip;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for ShaderResoruceView!");
			break;
		}
			ID3D11Resource* textureresource=nullptr;
			if (texture1D != nullptr)
				textureresource = texture1D;
			else if (texture2D != nullptr)
				textureresource = texture2D;
			else if (texture3D != nullptr)
				textureresource = texture3D;
			// Create ShaderResouceView
			HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
				textureresource, &srvDesc, &pShaderView
			);
			if (result != S_OK)
			{
				QCAT_CORE_ERROR("Texture View Contruction Failed!");
			}
			if (srcTexDesc.MipLevels > 0 && (srcTexDesc.bindFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
				QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pShaderView.Get());
	}
	DX11ShaderView::~DX11ShaderView()
	{
	}

	void DX11ShaderView::Bind(uint32_t slot, ShaderType type) const
	{
		QCAT_PROFILE_FUNCTION();
		switch (type)
		{
		case ShaderType::VS:
			QGfxDeviceDX11::GetInstance()->GetContext()->VSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::GS:
			QGfxDeviceDX11::GetInstance()->GetContext()->GSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::PS:	
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		case ShaderType::CS:
			QGfxDeviceDX11::GetInstance()->GetContext()->CSSetShaderResources(slot, 1u, pShaderView.GetAddressOf());
			break;
		}
	}

	DX11RenderTargetView::DX11RenderTargetView(TextureType type, Ref<Texture>& texture, TextureFormat format, uint32_t startMip,  uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);

		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D11_RTV_DIMENSION dstDimension = Utils::GetRTVDimensionFromType(type, multisampled);

		bool pass = true;
		if (srcTexDesc.MipLevels < startMip )
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("TextureView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		rtvDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		rtvDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;

		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layercount = numlayer;
		switch (srcTexDesc.Type)
		{
		case TextureType::Texture1D:
			rtvDesc.Texture1D.MipSlice = startMip;
			break;
		case TextureType::Texture1DArray:
			rtvDesc.Texture1DArray.MipSlice = startMip;
			rtvDesc.Texture1DArray.ArraySize = numlayer;
			rtvDesc.Texture1DArray.FirstArraySlice = startLayer;
			break;
		case TextureType::Texture2D:
			if (!multisampled)
			{
				rtvDesc.Texture2D.MipSlice = startMip;
			}
			break;
		case TextureType::Texture2DArray:		
			if (type == TextureType::Texture2D)
				layercount = 1;
			if (!multisampled)
			{
				rtvDesc.Texture2DArray.MipSlice = startMip;
				rtvDesc.Texture2DArray.ArraySize = layercount;
				rtvDesc.Texture2DArray.FirstArraySlice = startLayer;
			}
			else
			{
				rtvDesc.Texture2DMSArray.ArraySize = layercount;
				rtvDesc.Texture2DMSArray.FirstArraySlice = startLayer;
			}
		case TextureType::Texture3D:
			rtvDesc.Texture3D.MipSlice = startMip;
			rtvDesc.Texture3D.FirstWSlice = startLayer;
			rtvDesc.Texture3D.WSize = numlayer;
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for RenderTargetView!");
			break;
		}
		ID3D11Resource* textureresource=nullptr;
		if (texture1D != nullptr)
			textureresource = texture1D;
		else if (texture2D != nullptr)
			textureresource = texture2D;
		else if (texture3D != nullptr)
			textureresource = texture3D;
		// Create ShaderResouceView
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateRenderTargetView(
			textureresource, &rtvDesc, &pRenderTargetView
		);
		if (result != S_OK)
		{
			QCAT_CORE_ERROR("RenderTargetView Contruction Failed!");
		}
	}
	DX11RenderTargetView::~DX11RenderTargetView()
	{
	}

	DX11DepthStencilView::DX11DepthStencilView(TextureType type, Ref<Texture>& texture, TextureFormat format, uint32_t startMip,  uint32_t startLayer, uint32_t numlayer)
	{
		Texture_Desc srcTexDesc = texture->GetDesc();
		DXGI_FORMAT srcTexFormat = Utils::GetDirectDataType(srcTexDesc.Format);
		DXGI_FORMAT viewTexFormat = Utils::GetDirectDataType(format);

		uint32_t srcDimension = Utils::GetDimensionFromType(srcTexDesc.Type);

		bool multisampled = srcTexDesc.SampleCount > 1;
		D3D11_DSV_DIMENSION dstDimension = Utils::GetDSVDimenstionFromType(type, multisampled);

		bool pass = true;
		if (srcTexDesc.MipLevels < startMip)
		{
			QCAT_CORE_ERROR("DepthStenilView Contruction Error : MipLevel is wrong value for OriginTexture");
			pass = false;
		}
		if (srcTexDesc.ArraySize < startLayer || srcTexDesc.ArraySize < startLayer + numlayer)
		{
			QCAT_CORE_ERROR("DepthStenilView Contruction Error : ArrayIndex is Over or nuimLayer is over!");
			pass = false;
		}
		dsvDesc.Format = Utils::MapTypeSRV(viewTexFormat);
		dsvDesc.ViewDimension = dstDimension;

		ID3D11Texture1D* texture1D = nullptr;
		ID3D11Texture2D* texture2D = nullptr;
		ID3D11Texture3D* texture3D = nullptr;
		switch (srcDimension)
		{
		case 1:		texture1D = (ID3D11Texture1D*)texture->GetTextureHandle(); break;
		case 2:		texture2D = (ID3D11Texture2D*)texture->GetTextureHandle(); break;
		case 3:		texture3D = (ID3D11Texture3D*)texture->GetTextureHandle(); break;
		default:
			break;
		}
		uint32_t layercount = numlayer;
		switch (srcTexDesc.Type)
		{
		case TextureType::Texture1D:
			dsvDesc.Texture1D.MipSlice = startMip;
			break;
		case TextureType::Texture1DArray:
			dsvDesc.Texture1DArray.MipSlice = startMip;
			dsvDesc.Texture1DArray.ArraySize = numlayer;
			dsvDesc.Texture1DArray.FirstArraySlice = startLayer;
			break;
		case TextureType::Texture2D:
			if (!multisampled)
				dsvDesc.Texture2D.MipSlice = startMip;
			break;
		case TextureType::Texture2DArray:
			if (type == TextureType::Texture2D)
				layercount = 1;
			if (!multisampled)
			{
				dsvDesc.Texture2DArray.MipSlice = startMip;
				dsvDesc.Texture2DArray.ArraySize = layercount;
				dsvDesc.Texture2DArray.FirstArraySlice = startLayer;
			}
			else
			{
				dsvDesc.Texture2DMSArray.ArraySize = layercount;
				dsvDesc.Texture2DMSArray.FirstArraySlice = startLayer;
			}
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for DepthStenilView!");
			break;
		}
		ID3D11Resource* textureresource=nullptr;
		if (texture1D != nullptr)
			textureresource = texture1D;
		else if (texture2D != nullptr)
			textureresource = texture2D;
		else if (texture3D != nullptr)
			textureresource = texture3D;
		// Create ShaderResouceView
		HRESULT result = QGfxDeviceDX11::GetInstance()->GetDevice()->CreateDepthStencilView(
			textureresource, &dsvDesc, &pDepthStencilView
		);
		if (result != S_OK)
		{
			QCAT_CORE_ERROR("DepthStencilView Contruction Failed!");
		}
	}

	DX11DepthStencilView::~DX11DepthStencilView()
	{
	}

}
