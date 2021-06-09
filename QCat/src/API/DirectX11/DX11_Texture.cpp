#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
#include "DXUtils.h"
#include "DX11_Sampler.h"
#include "QCat/Uitiliy/Float16.h"
namespace QCat
{
	DX11Texture2D::DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gamacorrection )
	{
		QCAT_PROFILE_FUNCTION();
		this->path = path;
		int width, height, channels;
		if(!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		auto begin = path.find_last_of('.');
		std::string extension = path.substr(begin + 1, path.length());
		void* stb_data=nullptr;
		void* pData = nullptr;
		Float16* float16Array=nullptr;
		unsigned char* UData = nullptr;
			if (extension != "hdr")
			{
				stb_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load Image!");

			}
			else
			{
				stb_data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load HDR image!");
				
				float16Array = new Float16[width * height * 4];
				for (unsigned int i=0,j= 0; j< width*height*3; i+=4,j+=3)
				{
					float16Array[i] = ((float*)stb_data)[j];
					float16Array[i+1] = ((float*)stb_data)[j+1];
					float16Array[i+2] = ((float*)stb_data)[j+2];
					float16Array[i + 3] = 1.0f;
				}
			}
		{
			if (extension != "hdr")
			{
				switch (channels)
				{
				case 4:
					m_dataFormat = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
					UData = new unsigned char[width * height * 4];
					for (int i=0;i < width * height*4; ++i )
						UData[i] = ((unsigned char*)stb_data)[i];
					break;
				case 3:
					m_dataFormat = gamacorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
					UData = new unsigned char[width * height * 4];
					for (int i=0,j = 0; j < width * height * 3; i+=4,j+=3)
					{
						UData[i]   = ((unsigned char*)stb_data)[j];
						UData[i+1] = ((unsigned char*)stb_data)[j+1];
						UData[i+2] = ((unsigned char*)stb_data)[j+2];
						UData[i + 3] = 1;
					}
					break;
				case 2:
					m_dataFormat = gamacorrection ? DXGI_FORMAT_R8G8_SNORM : DXGI_FORMAT_R8G8_UNORM;
					UData = new unsigned char[width * height * 4];
					for (int i = 0, j = 0; j < width * height * 2; i += 4, j += 2)
					{
						UData[i] = ((unsigned char*)stb_data)[j];
						UData[i + 1] = ((unsigned char*)stb_data)[j + 1];
						UData[i + 2] = 1;
						UData[i + 3] = 1;
					}
					break;
				case 1:
					UData = new unsigned char[width * height];
					m_dataFormat = DXGI_FORMAT_R8_UNORM;
					for (int i = 0; i < width * height; ++i)
						UData[i] =   ((unsigned char*)stb_data)[i];
					break;
				}
				pData = UData;
			}
			else
			{
				switch (channels)
				{
				case 4: 
					float16Array = new Float16[width * height * 4];
					for (unsigned int i = 0;i < width * height * 4; i++)
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
				m_dataFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
				//m_dataFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
		m_width = width;
		m_height = height;
	
		// Texture Description
		textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		if(mipLevel > 0 )
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(m_dataFormat);
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;

		Invalidate(pData);
		sampler = DX11Sampler::Create(desc);

		// free loaded image
		if (float16Array != nullptr)
			delete[] float16Array;
		if (UData != nullptr)
			delete[] UData;
		stbi_image_free(stb_data);

	}
	DX11Texture2D::DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
		:m_width(width),m_height(height)
	{
		m_dataFormat = Utils::GetDirectDataType(format);

		textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
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

		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Type = TextureType::Texture2D;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;

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
	void DX11Texture2D::SetData(void* data, unsigned int size)
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
		m_width = width;
		m_height = height;
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
		textureDesc.SampleDesc.Count = samples;
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
		if (x >= 0 && x <= m_width && y >= 0 && y <= m_height)
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
		//srvDesc.Texture2D.MostDetailedMip = this->desc.MipLevels == 0 ? this->desc.MipLevels : this->desc.MipLevels - 1;
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

		
		if (!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);

		stbi_uc* data[6];
		int width, height, channels;
		if (gammaCorrection)
		{
			m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		}
		else
		{
			m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		for (int i = 0; i < imgPathes.size(); ++i)
		{
			
			data[i] = stbi_load(imgPathes[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);

			QCAT_CORE_ASSERT(data, "Failed to load Image!");
			m_width = width;
			m_height = height;
		}
	
		// Texture Description
		textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE ;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = Utils::GetTextureFormatFromDX(m_dataFormat);
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;
		// Create Texture and update it
		D3D11_SUBRESOURCE_DATA subResource[6];
		for (int i = 0; i < 6; ++i)
		{
			subResource[i].pSysMem = data[i];
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
			stbi_image_free(data[i]);
		}

		sampler = DX11Sampler::Create(desc);
	}
	DX11TextureCube::DX11TextureCube(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, void* pData)
		:m_width(width),m_height(height)
	{
		m_dataFormat = Utils::GetDirectDataType(format);
		// Texture Description
		textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 6;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Format = format;
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromDX(textureDesc.Format);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;
		this->desc.ArraySize = textureDesc.ArraySize;

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
	void DX11TextureCube::SetData(void* pData, unsigned int size)
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
		textureDesc.SampleDesc.Count = samples;
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
		if (x >= 0 && x <= m_width && y >= 0 && y <= m_height)
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
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		//srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
	}
}
