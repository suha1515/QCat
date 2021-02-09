#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
namespace QCat
{
	DX11Texture2D::DX11Texture2D(unsigned int width, unsigned int height)
		:m_width(width),m_height(height)
	{
		QCAT_PROFILE_FUNCTION();

		m_dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// Texture Description
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = m_width;
		textureDesc.Height = m_height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);

		// Texture resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0; // mip level
		srvDesc.Texture2D.MipLevels = -1;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);
		// Generate Mip
		QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		// free loaded image
	}
	DX11Texture2D::DX11Texture2D(const std::string& path, bool gamacorrection )
	{
		QCAT_PROFILE_FUNCTION();

		int width, height, channels;
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
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = m_dataFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
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
		srvDesc.Texture2D.MostDetailedMip = 0; // mip level
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

		// Create ShaderResouceView
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		);

		//Set SamplerState
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		if (channels > 3)
		{
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
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

		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateSamplerState(&samplerDesc, &pSamplerState);


		// Generate Mip
		QGfxDeviceDX11::GetInstance()->GetContext()->GenerateMips(pTextureView.Get());
		// free loaded image
		stbi_image_free(data);
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	void DX11Texture2D::SetData(void* data, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = m_dataFormat == DXGI_FORMAT_R8G8B8A8_UNORM ? 4 : 3;
		QCAT_CORE_ASSERT(size == m_width * m_height * bpc, "Data must be entire texture!");
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, size * sizeof(unsigned int), 0u);
	}
	void DX11Texture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();

		if (pSamplerState)
			QGfxDeviceDX11::GetInstance()->GetContext()->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
}
