#include "qcpch.h"
#include "DX11_Texture.h"
#include "stb_image.h"
namespace QCat
{
	DX11Texture2D::DX11Texture2D(const std::string& path)
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		QCAT_CORE_ASSERT(data, "Failed to load Image!");
		m_width = width;
		m_height = height;

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

		// Create Texture and update it
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		QGfxDeviceDX11::GetInstance()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture);
		QGfxDeviceDX11::GetInstance()->GetContext()->UpdateSubresource(pTexture.Get(), 0u, nullptr, data, m_width *sizeof(unsigned int), 0u);
		
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
		stbi_image_free(data);
	}
	DX11Texture2D::~DX11Texture2D()
	{
	}
	void DX11Texture2D::Bind(unsigned int slot) const
	{
		QGfxDeviceDX11::GetInstance()->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
}