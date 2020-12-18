#include "qcpch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h> 
#include "stb_image.h"

namespace QCat
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		QCAT_CORE_ASSERT(data, "Failed to load Image!");
		m_width = width;
		m_height = height;

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		glTextureStorage2D(m_renderID, 1, GL_RGB8, m_width, m_height);

		// Texture Filtering
		glTextureParameteri(m_renderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_renderID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Upload Texture
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE,data);

		// free loaded image
		stbi_image_free(data);

	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		// 1st parameter is for slot
		glBindTextureUnit(slot, m_renderID);
	}
}