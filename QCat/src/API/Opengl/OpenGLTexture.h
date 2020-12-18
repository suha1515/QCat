#pragma once
#include "Renderer/Texture.h"

namespace QCat
{
	class OpenGLTexture2D :public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const { return m_width; }
		virtual unsigned int GetHeight() const {return m_height;}

		virtual void Bind(unsigned int slot=0) const;
	private:
		std::string m_path;
		unsigned int m_width,m_height;
		unsigned int m_renderID;
	};
}