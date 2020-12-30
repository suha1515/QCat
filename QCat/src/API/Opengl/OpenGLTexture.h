#pragma once
#include "QCat/Renderer/Texture.h"
#include <glad/glad.h> 
namespace QCat
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(unsigned int width,unsigned int height);

		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override {return m_height;}

		virtual void SetData(void* pData, unsigned int size) override;

		virtual void Bind(unsigned int slot=0) const override;
	private:
		std::string m_path;
		unsigned int m_width,m_height;
		unsigned int m_renderID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}