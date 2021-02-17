#include "qcpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace QCat
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool flip, bool gammaCorrection)
		:m_path(path)
	{
		QCAT_PROFILE_FUNCTION();

		int width, height, channels;
		if(!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			QCAT_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		
		QCAT_CORE_ASSERT(data, "Failed to load Image!");
		m_width = width;
		m_height = height;

		GLenum imageformat=0, dataformat=0;
		switch (channels)
		{
		case 4:
			imageformat = gammaCorrection ? GL_SRGB8_ALPHA8: GL_RGBA8;
			dataformat = GL_RGBA;
			break;
		case 3:
			imageformat = gammaCorrection ? GL_SRGB8 : GL_RGB8;
			dataformat = GL_RGB;
			break;
		}
		m_InternalFormat = imageformat;
		m_DataFormat = dataformat;
		QCAT_CORE_ASSERT(imageformat & dataformat, "Format is not supported!");

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		glTextureStorage2D(m_renderID, 1, imageformat, m_width, m_height);

		// Texture Filtering
		glTextureParameteri(m_renderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_renderID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (m_DataFormat == GL_RGB)
		{
			glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else if (m_DataFormat == GL_RGBA)
		{
			glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		


		// Upload Texture
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, dataformat, GL_UNSIGNED_BYTE,data);

		// free loaded image
		stbi_image_free(data);

	}
	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height)
		:m_width(width),m_height(height)
	{
		QCAT_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		glTextureStorage2D(m_renderID, 1, m_InternalFormat, m_width, m_height);

		// Texture Filtering
		glTextureParameteri(m_renderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_renderID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_renderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::SetData(void* pData, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = m_DataFormat == GL_RGBA ? 4 : 3;
		QCAT_CORE_ASSERT(size == m_width * m_height * bpc,"Data must be entire texture!");
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_DataFormat, GL_UNSIGNED_BYTE, pData);
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();

		// 1st parameter is for slot
		glBindTextureUnit(slot, m_renderID);
	}
}