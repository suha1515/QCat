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
	class DX11Texture1D :public Texture1D
	{

	};
	class DX11Texture1DArray : public Texture1DArray
	{

	};
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1, void* pData = nullptr);
		DX11Texture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip=false,bool gamacorrection = false);
		virtual ~DX11Texture2D();

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override { return desc.Height; }
		virtual void* GetTexture() const override { return (void*)pTextureView.Get(); }
		virtual void* GetTextureHandle() const override { return (void*)pTexture.Get(); };

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) {};
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
		std::string m_path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
		D3D11_TEXTURE2D_DESC textureDesc;
	};
	class DX11Texture2DArray : public Texture2DArray
	{
	public:
		DX11Texture2DArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel = 1, unsigned int samples = 1);
		DX11Texture2DArray(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false, bool gamacorrection = false);
		virtual ~DX11Texture2DArray() {};

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override { return desc.Height; }
		virtual void* GetTexture() const override { return (void*)pTextureView.Get(); }
		virtual void* GetTextureHandle() const override { return (void*)pTexture.Get(); };

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) {};

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return pTexture.Get() == ((DX11Texture2DArray&)other).pTexture.Get();
		}
		void Validate(std::vector<void*> pData = {});
		void Update(void* pData, uint32_t index, uint32_t mipSlice);
	private:
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

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override { return desc.Height; }
		virtual void* GetTexture() const override { return (void*)pTextureView.Get(); }
		virtual void* GetTextureHandle() const override { return (void*)pTexture.Get(); };

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) {};
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
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
		D3D11_TEXTURE2D_DESC textureDesc;
	};

	class DX11TextureUtility : public TextureUtility
	{
	public:
		DX11TextureUtility()=default;
		virtual ~DX11TextureUtility() = default;

		virtual void CopyTexture2D_(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion) override
		{};
		virtual void CopyCubemapFace2D_(Ref<Texture>& srcCubeMap, Ref<Texture>& dstTex, uint32_t index, uint32_t mipLevl, QCAT_BOX boxregion) override
		{};
	private:

	};

	class DX11ShaderView : public TextureShaderView
	{
	public:
		DX11ShaderView() = default;
		DX11ShaderView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer);
		virtual ~DX11ShaderView() override;

		virtual void Bind(uint32_t slot, ShaderType type) const override;

		virtual void* GetTextureView() const override { return (void*)pShaderView.Get(); };
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderView = nullptr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	};
	class DX11RenderTargetView : public RenderTargetView
	{
	public:
		DX11RenderTargetView() = default;
		DX11RenderTargetView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip,uint32_t startLayer, uint32_t numlayer);
		virtual ~DX11RenderTargetView() override;
		virtual void Bind(uint32_t framebufferid, AttachmentType type) override {};
		virtual void* GetTextureView() const override { return (void*)pRenderTargetView.Get(); };
		void Clear(const glm::vec4& color);
	private:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView = nullptr;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	};
	class DX11DepthStencilView : public DepthStencilView
	{
	public:
		DX11DepthStencilView() = default;
		DX11DepthStencilView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip,uint32_t startLayer, uint32_t numlayer);
		virtual ~DX11DepthStencilView() override;
		virtual void Bind(uint32_t framebufferid, AttachmentType type) override {};
		virtual void* GetTextureView() const override { return (void*)pDepthStencilView.Get(); };
		void Clear(float depth = 1.0f, float stencil = 0.0f);
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = nullptr;
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	};
	
}
