#pragma once
#include "QCat/Renderer/Texture.h"
#include <glad/glad.h> 
namespace QCat
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, bool flip =false, bool gammaCorrection = false);
		OpenGLTexture2D(unsigned int width,unsigned int height);

		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override {return m_height;}
		virtual void*       GetTexture() const override { return (void*)m_renderID; }

		virtual void SetData(void* pData, unsigned int size) override;

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
		GLenum m_InternalFormat, m_DataFormat;
	};

	class OpenGLCubeMapTexture : public TextureCube
	{
	public:
		OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, bool flip = false, bool gammaCorrection = false);
		virtual ~OpenGLCubeMapTexture();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }
		virtual void* GetTexture() const override { return (void*)m_renderID; }

		virtual void SetData(void* pData, unsigned int size) override;
		virtual void Bind(unsigned int slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_renderID == ((OpenGLCubeMapTexture&)other).m_renderID;
		}
	private:
		unsigned int m_width, m_height;
		unsigned int m_renderID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}