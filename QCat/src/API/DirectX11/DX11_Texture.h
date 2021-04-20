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
		DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1, void* pData = nullptr);
		DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip=false,bool gamacorrection = false);
		virtual ~DX11Texture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)pTextureView.Get(); }

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) override;

		virtual bool operator==(const Texture& other) const override
		{
			return pTexture.Get() == ((DX11Texture2D&)other).pTexture.Get();
		}

		ID3D11Texture2D* GetDXTexture() { return pTexture.Get(); }

		void Invalidate(void* pData);
	private:
		int samples = 0;
		std::string m_path;
		unsigned int m_width, m_height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
		D3D11_TEXTURE2D_DESC textureDesc;
	};
	class DX11TextureCube : public TextureCube
	{
	public:
		DX11TextureCube(const std::vector<std::string>& imgPathes, Sampler_Desc desc, unsigned int mipLevel = 1, bool flip = false, bool gammaCorrection = false);
		DX11TextureCube(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, void* pData = nullptr);

		~DX11TextureCube() = default;

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)pTextureView.Get(); }

		virtual void GenerateMipMap() override;

		virtual void SetData(void* pData, unsigned int size) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void Bind(unsigned int slot = 0) const override;

		virtual void ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata) override;

		virtual bool operator==(const Texture& other) const override
		{
			return pTexture.Get() == ((DX11TextureCube&)other).pTexture.Get();
		}

		ID3D11Texture2D* GetDXTexture() { return pTexture.Get(); }
		void Invalidate(void* pData);
	private:
		int samples = 0;
		unsigned int m_width, m_height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
		D3D11_TEXTURE2D_DESC textureDesc;
	};
}
