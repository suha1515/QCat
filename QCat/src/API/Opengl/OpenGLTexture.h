#pragma once
#include "QCat/Renderer/Texture.h"
#include <glad/glad.h>
namespace QCat
{
	class OpenGLTexture1D :public Texture1D
	{

	};
	class OpenGLTexture1DArray : public Texture1DArray
	{

	};
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(Texture_Desc desc, Sampler_Desc sampDesc, void* pData);
		OpenGLTexture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel=1,unsigned int samples=1, bool flip = false, bool gammaCorrection = false);
		OpenGLTexture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1, void* pData = nullptr);

		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override {return desc.Height;}
		virtual void*       GetTexture() const override { return (void*)m_renderID; }
		virtual void* GetTextureHandle() const override { return (void*)m_renderID; }

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t miplevel,uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, uint32_t bufSize, void* pixels) override;
		virtual void ClearData(uint32_t miplevel, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, const void* data) override;

		virtual void Bind(unsigned int slot=0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLTexture2D&)other).m_renderID;
		}
		void Validate( void* pData);
	private:
		unsigned int m_renderID=0;
		bool flip = false;
		GLenum m_InternalFormat=0, m_Format=0,m_DataFormat=0;
	};
	class OpenGlTexture2DArray : public Texture2DArray
	{
	public:
		OpenGlTexture2DArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel = 1, unsigned int samples = 1);
		OpenGlTexture2DArray(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel = 1, unsigned int samples = 1, bool flip = false, bool gamacorrection = false);
	
		virtual ~OpenGlTexture2DArray();

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override { return desc.Height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }
		virtual void* GetTextureHandle() const override { return (void*)m_renderID; }

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) {};

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGlTexture2DArray&)other).m_renderID;
		}
		void Validate(void* pData = nullptr, uint32_t index = 0);
	private:
		unsigned int m_renderID = 0;
		GLenum m_InternalFormat = 0, m_Format = 0, m_DataFormat = 0;
	};
	class OpenGLCubeMapTexture : public TextureCube
	{
	public:
		OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels=1, bool flip = false, bool gammaCorrection = false);
		OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width,unsigned int height, unsigned int mipLevels = 1);

		virtual ~OpenGLCubeMapTexture();

		void Validate();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }
		virtual void* GetTextureHandle() const override { return (void*)m_renderID; }

		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata) override {};

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLCubeMapTexture&)other).m_renderID;
		}
	private:
		unsigned int m_width, m_height;
		unsigned int m_renderID;
		unsigned int m_mipLevel, m_samples;
		bool flip = false, gammaCorrection = false;
		GLenum m_InternalFormat, m_Format, m_DataFormat;
	};
	class OpenGLCubeMapArray : public TextureCubeArray
	{
	public:
		OpenGLCubeMapArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel = 1, unsigned int samples = 1);

		virtual ~OpenGLCubeMapArray();

		void Validate();

		virtual unsigned int GetWidth() const override { return desc.Width; }
		virtual unsigned int GetHeight() const override { return desc.Height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }
		virtual void* GetTextureHandle() const override { return (void*)m_renderID; }
		virtual void GenerateMipMap() override {};

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override {};
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override {};
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override {};

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLCubeMapArray&)other).m_renderID;
		}
	private:
		unsigned int m_renderID;
		GLenum m_InternalFormat, m_Format, m_DataFormat;
	};

	class OpenGLTextureUtility : public TextureUtility
	{
	public:
		OpenGLTextureUtility();
		virtual ~OpenGLTextureUtility();

		virtual void CopyTexture2D_(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion) override;
		virtual void CopyCubemapFace2D_(Ref<Texture>& srcCubeMap, Ref<Texture>& dstTex, uint32_t index, uint32_t mipLevl, QCAT_BOX boxregion)override;
	private:
		uint32_t m_RendererID;

	};
	class OpenGLTextureShaderView : public TextureShaderView
	{
	public:
		OpenGLTextureShaderView() = default;
		OpenGLTextureShaderView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer);
		
		virtual ~OpenGLTextureShaderView() override;
		virtual void* GetTextureView() const override { return (void*)m_renderID; };
		virtual void Bind(uint32_t slot, ShaderType type) const override;
	private:
		uint32_t m_renderID;
	};
	class OpenGLRenderTargetView : public RenderTargetView
	{
	public:
		OpenGLRenderTargetView() = default;
		OpenGLRenderTargetView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t startLayer, uint32_t numlayer);

		virtual ~OpenGLRenderTargetView() override;
		virtual void Bind(uint32_t framebufferid, AttachmentType type) override;
		virtual void* GetTextureView() const override { return (void*)m_renderID; };

	private:
		uint32_t m_renderID;
		uint32_t m_startMip;
	};
	class OpenGLDepthStencilView : public DepthStencilView
	{
	public:
		OpenGLDepthStencilView() = default;
		OpenGLDepthStencilView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t startLayer, uint32_t numlayer);

		virtual ~OpenGLDepthStencilView() override;
		virtual void Bind(uint32_t framebufferid, AttachmentType type) override;
		virtual void* GetTextureView() const override { return (void*)m_renderID; };
	private:
		uint32_t m_renderID;
		uint32_t m_startMip;

	};
}