#pragma once
#include "QCat/Renderer/Texture.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	// In DirectX there are Texture1D,2D,3D
	// and Texture2D means 2dimension texture
	// if you want to use TextureArray about Texture you need to distinguishi Texture2D[...] and Texture2DArray
	// Texture2D[] is each Textures dont have to be same foramt or data but Texture2DArray must be same
	// so when you use Texute2D[] in your hlsl code each Texture2D has own slot for using
	// Like Texuture2D[0]'s slot 0 and Texture2D[1]'s slot is 1
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(unsigned int width,unsigned int height);
		DX11Texture2D(const std::string& path);
		virtual ~DX11Texture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }

		virtual void SetData(void* data, unsigned int size) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_path == ((DX11Texture2D&)other).m_path;
		}

	private:
		std::string m_path;
		unsigned int m_width, m_height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
	};
}
