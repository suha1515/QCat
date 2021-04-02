#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
#include "DXUtils.h"
#include "DX11_Sampler.h"
namespace QCat
{
	DX11Texture2D::DX11Texture2D(Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel , unsigned int samples )
		:m_width(width),m_height(height)
	{
		QCAT_PROFILE_FUNCTION();

		m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// Texture Description
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = mipLevel;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = samples;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		if (mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = mipLevel == 0 ? mipLevel :mipLevel - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		// free loaded image
		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gamacorrection )
	{
		QCAT_PROFILE_FUNCTION();

		int width, height, channels;
		if(!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (gamacorrection)
		{
			m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		}
		else
		{
			m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		
		QCAT_CORE_ASSERT(data, "Failed to load Image!");
		m_width = width;
		m_height = height;
		
		// Texture Description
		D3D11_TEXTURE2D_DESC textureDesc = {};
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
		if(mipLevel > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		// Create Texture and update it
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = data;
		subResource.SysMemPitch = textureDesc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, &subResource, &pTexture);
		//QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, m_width *sizeof(unsigned int), 0u);
		
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = mipLevel == 0 ? mipLevel : mipLevel - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		//Set SamplerState
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		if (channels > 3)
		{
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		else
		{
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		// free loaded image
		stbi_image_free(data);
		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
		:m_width(width),m_height(height)
	{
		m_dataFormat = Utils::GetDirectDataType(format);

		D3D11_TEXTURE2D_DESC textureDesc = {};
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

		D3D11_SUBRESOURCE_DATA data = {};
		
		if (pData != nullptr)
		{
			unsigned long long datasize = Utils::GetDataSize(format);
			data.pSysMem = pData;
			data.SysMemPitch = 32;
			data.SysMemSlicePitch = 0;
			//QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, pData, m_width * 8, 0u);
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, &data, &pTexture);

	
		
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapUsageColored(format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = mipLevel == 0 ? mipLevel : mipLevel - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::DX11Texture2D(D3D11_TEXTURE2D_DESC textureDesc, Sampler_Desc desc, bool flip, bool gammaCorrection)
	{

		m_width = textureDesc.Width;
		m_height = textureDesc.Height;
		m_dataFormat = textureDesc.Format;
		samples = textureDesc.SampleDesc.Count;
		if (textureDesc.MipLevels > 0 && !(textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, 0, &pTexture);
		//QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, m_width *sizeof(unsigned int), 0u);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = Utils::MapTypeSRV(textureDesc.Format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MipLevels = -1;
		
		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		//Set SamplerState
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;


		// Generate Mip
		if (textureDesc.MipLevels > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
		 QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());

		sampler = DX11Sampler::Create(desc);
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	void DX11Texture2D::SetData(void* data, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		//unsigned int bpc = m_dataFormat == DXGI_FORMAT_R8G8B8A8_UNORM ? 4 : 3;
		//QCAT_CORE_ASSERT(size == m_width * m_height * bpc, "Data must be entire texture!");
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, size, 0u);
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
		D3D11_TEXTURE2D_DESC textureDesc = {};
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

		// Create Texture and update it
		D3D11_SUBRESOURCE_DATA subResource[6];
		for (int i = 0; i < 6; ++i)
		{
			subResource[i].pSysMem = data[i];
			subResource[i].SysMemPitch = textureDesc.Width * 4;
			subResource[i].SysMemSlicePitch = 0;
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc,subResource, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		// Generate CubeMap mips
		if (mipLevel > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());

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
		D3D11_TEXTURE2D_DESC textureDesc = {};
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

		// Create Texture and update it
		if (pData != nullptr)
		{
			D3D11_SUBRESOURCE_DATA subResource[6];
			for (int i = 0; i < 6; ++i)
			{
				subResource[i].pSysMem = pData;
				subResource[i].SysMemPitch = textureDesc.Width * Utils::GetDataSize(format);
				subResource[i].SysMemSlicePitch = 0;
			}
			QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, subResource, &pTexture);
		}
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		// Generate CubeMap mips
		if (mipLevel > 0 && (textureDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS))
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());

		sampler = DX11Sampler::Create(desc);
	}
	DX11TextureCube::DX11TextureCube(D3D11_TEXTURE2D_DESC textureDesc, Sampler_Desc desc, bool flip, bool gammaCorrection)
	{
		m_dataFormat = textureDesc.Format;
		m_width = textureDesc.Width;
		m_height = textureDesc.Height;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, 0, &pTexture);
		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MostDetailedMip = textureDesc.MipLevels == 0 ? textureDesc.MipLevels : textureDesc.MipLevels - 1;
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		// Generate Mip
		if (textureDesc.MipLevels > 0)
			QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());

		sampler = DX11Sampler::Create(desc);
	}
	void DX11TextureCube::SetData(void* pData, unsigned int size)
	{
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
}
