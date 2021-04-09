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
		OpenGLTexture2D(Sampler_Desc desc, unsigned int width,unsigned int height, unsigned int mipLevel = 1, unsigned int samples = 1);

		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override {return m_height;}
		virtual void*       GetTexture() const override { return (void*)m_renderID; }

		virtual void SetData(void* pData, unsigned int size) override;

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
	private:
		unsigned int m_width,m_height;
		unsigned int m_renderID;
		unsigned int mipLevel,samples;
		bool flip=false, gammaCorrection = false;
		GLenum m_InternalFormat, m_Format;
	};

	class OpenGLCubeMapTexture : public TextureCube
	{
	public:
		OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels=1, bool flip = false, bool gammaCorrection = false);
		OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width,unsigned int height, unsigned int mipLevels = 1, void* pData = nullptr);

		virtual ~OpenGLCubeMapTexture();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }

		virtual void SetData(void* pData, unsigned int size) override;

		virtual void ReadData(uint32_t face, uint32_t x, uint32_t y, const void* outdata) {};

		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLCubeMapTexture&)other).m_renderID;
		}
	private:
		unsigned int m_width, m_height;
		unsigned int mipLevel;
		unsigned int m_renderID;
		GLenum m_InternalFormat[6], m_Format[6];
		bool flip = false, gammaCorrection = false;
	};
}