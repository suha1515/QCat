#pragma once
#include "QCat/Renderer/Texture.h"
#include <glad/glad.h> 
namespace QCat
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, bool gammaCorrection = false);
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
		virtual std::string GetPath() const override { return m_path; }
	private:
		std::string m_path;
		unsigned int m_width,m_height;
		unsigned int m_renderID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}