#pragma once
#include "QCat/Renderer/Texture.h"
#include <glad/glad.h>
namespace QCat
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel=1,unsigned int samples=1, bool flip = false, bool gammaCorrection = false);
		OpenGLTexture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1, void* pData = nullptr);

		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override {return m_height;}
		virtual void*       GetTexture() const override { return (void*)m_renderID; }
		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t x, uint32_t y, const void* outdata) {};

		virtual void Bind(unsigned int slot=0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLTexture2D&)other).m_renderID;
		}
		//virtual Texture& operator=(const Texture& other) override
		//{
		//	m_width = ((OpenGLTexture2D&)other).m_width;
		//	m_height = ((OpenGLTexture2D&)other).m_height;
		//	m_InternalFormat = ((OpenGLTexture2D&)other).m_InternalFormat;
		//	m_DataFormat = ((OpenGLTexture2D&)other).m_DataFormat;
		//}
		void Validate(GLenum format, GLenum internalFormat, GLenum dataFormat, uint32_t width, uint32_t heigth, uint32_t mipLevels, uint32_t samples, void* pData);
	private:
		unsigned int m_width,m_height;
		unsigned int m_renderID=0;
		unsigned int m_mipLevel,m_samples;
		bool flip=false, gammaCorrection = false;
		GLenum m_InternalFormat, m_Format,m_DataFormat;
		Sampler_Desc  smpdesc;
	};

	class OpenGLCubeMapTexture : public TextureCube
	{
	public:
		OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels=1, bool flip = false, bool gammaCorrection = false);
		OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width,unsigned int height, unsigned int mipLevels = 1, void* pData = nullptr);

		virtual ~OpenGLCubeMapTexture();

		void Validate(GLenum target, GLenum format, GLenum internalFormat, GLenum dataFormat, uint32_t width, uint32_t heigth, uint32_t mipLevels, uint32_t samples);

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }
		virtual void GenerateMipMap() override;

		virtual void SetData(void* data, unsigned int size, uint32_t textureindex = 0) override;
		virtual void GetData(void* data, uint32_t mipLevel = 0, uint32_t textureindex = 0) override;
		virtual void SetSize(uint32_t width, uint32_t height, uint32_t depth = 0) override;

		virtual void ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata) {};

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
		Sampler_Desc  smpdesc;
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
}