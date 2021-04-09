#include "qcpch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"
#include "API/Opengl/OpenglUtils.h"
#include "API/Opengl/Opengl_Sampler.h"

namespace QCat
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples , bool flip, bool gammaCorrection)
		:mipLevel(mipLevel),samples(samples),flip(flip),gammaCorrection(gammaCorrection)
	{
		QCAT_PROFILE_FUNCTION();
		auto begin = path.find_last_of('.');
		std::string extension = path.substr(begin + 1, path.length());
		int width, height, channels;
		if(!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		void* pData;
		//stbi_uc* data = nullptr;
		//float* fdata = nullptr;
		{
			QCAT_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			if (extension != "hdr")
			{
				pData = stbi_load(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(pData, "Failed to load Image!");
			}
			else
			{
				pData = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(pData, "Failed to load HDR image!");
			}
		}
		m_width = width;
		m_height = height;

		GLenum Internalformat=0, format=0,dataFormat=0;
		{
			if (extension != "hdr")
			{
				switch (channels)
				{
				case 4:
					Internalformat = gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8;
					format = GL_RGBA;
					break;
				case 3:
					Internalformat = gammaCorrection ? GL_SRGB8 : GL_RGB8;
					format = GL_RGB;
					break;
				case 1:
					Internalformat = GL_R32F;
					format = GL_RED;
					break;
				}
				dataFormat = GL_UNSIGNED_BYTE;
			}
			else
			{
				Internalformat = GL_RGB16F;
				format = GL_RGB;
				dataFormat = GL_FLOAT;
			}
			
		}
		m_InternalFormat = Internalformat;
		m_Format = format;
		QCAT_CORE_ASSERT(Internalformat & format, "Format is not supported!");

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);

		bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);
		
		// Upload Texture
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, dataFormat, pData);

		if (mipLevel > 1)
			glGenerateTextureMipmap(m_renderID);
		// free loaded image
		stbi_image_free(pData);

		sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
		:m_width(width),m_height(height), mipLevel(mipLevel), samples(samples)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);

		m_InternalFormat = Utils::GetTextureInternalFormat(format);
		m_Format = Utils::GetTextureFormat(format);

		bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);

		GLenum dataFormat = Utils::GetTextureDataFormat(format);
		if(pData!=nullptr)
			glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, dataFormat,pData);

		if(mipLevel>1)
			glGenerateTextureMipmap(m_renderID);

		sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::OpenGLTexture2D(Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel,unsigned int samples)
		:m_width(width),m_height(height),mipLevel(mipLevel),samples(samples)
	{
		QCAT_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8, m_Format = GL_RGBA;

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);

		bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);

		sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::SetData(void* pData, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = m_Format == GL_RGBA ? 4 : 3;
		QCAT_CORE_ASSERT(size == m_width * m_height * bpc,"Data must be entire texture!");
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, GL_UNSIGNED_BYTE, pData);
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();

		// 1st parameter is for slot
		glBindTextureUnit(slot, m_renderID);
		sampler->Bind(slot);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels,bool flip , bool gammaCorrection)
		:flip(flip),gammaCorrection(gammaCorrection)
	{
		QCAT_PROFILE_FUNCTION();

		uint32_t textureID;
		glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);

		if (!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		
		for (uint32_t i = 0; i < imgPathes.size(); ++i)
		{

			stbi_uc* data = nullptr;
			GLenum imageformat = 0,format = 0;
			{
				QCAT_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
				data = stbi_load(imgPathes[i].c_str(), &width, &height, &channels, 0);
				switch (channels)
				{
				case 4:
					imageformat = gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8;
					format = GL_RGBA;
					break;
				case 3:
					imageformat = gammaCorrection ? GL_SRGB8 : GL_RGB8;
					format = GL_RGB;
					break;
				}
			}
			QCAT_CORE_ASSERT(data, "Failed to load Image!");
			m_width = width;
			m_height = height;
			m_InternalFormat[i] = imageformat;
			m_Format[i] = format;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,mipLevels - 1, m_InternalFormat[i], width, height, 0, m_Format[i], GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		glGenerateTextureMipmap(m_renderID);

		sampler = OpenGLSampler::Create(desc);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevels, void* pData)
		:m_width(width),m_height(height), mipLevel(mipLevels)
	{
		glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);

		GLenum internalFormat= Utils::GetTextureInternalFormat(format);
		GLenum textureformat = Utils::GetTextureFormat(format);
		GLenum dataFormat = Utils::GetTextureDataFormat(format);
		for (int i = 0; i < 6; ++i)
		{
			m_InternalFormat[i] = internalFormat;
			m_Format[i] = textureformat;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevels - 1, m_InternalFormat[i], width, height, 0, m_Format[i], dataFormat, pData);

		}
		glGenerateTextureMipmap(m_renderID);
		/*glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
		sampler = OpenGLSampler::Create(desc);
	}
	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLCubeMapTexture::SetData(void* pData, unsigned int size)
	{
	}
	void OpenGLCubeMapTexture::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();

		// 1st parameter is for slot
		glBindTextureUnit(slot, m_renderID);
		//glActiveTexture(GL_TEXTURE0+slot);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		sampler->Bind(slot);
	}
	
}