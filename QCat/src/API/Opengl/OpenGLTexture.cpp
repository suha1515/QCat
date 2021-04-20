#include "qcpch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"
#include "API/Opengl/OpenglUtils.h"
#include "API/Opengl/Opengl_Sampler.h"
#include "QCat/Uitiliy/Float16.h"
namespace QCat
{
	namespace Utils
	{
		void SetTextureParameter(GLenum target, Sampler_Desc desc)
		{
			Utils::SetTextureFilter(target, GL_TEXTURE_MIN_FILTER, desc.MIN, desc.MIP);
			Utils::SetTextureFilter(target, GL_TEXTURE_MAG_FILTER, desc.MAG);
			Utils::SetTextureWrapMethod(target, GL_TEXTURE_WRAP_S, desc.addressU);
			Utils::SetTextureWrapMethod(target, GL_TEXTURE_WRAP_T, desc.addressV);
			Utils::SetTextureWrapMethod(target, GL_TEXTURE_WRAP_R, desc.addressW);

			if (desc.mode == FilterMode::COMPARISON)
			{
				glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, Utils::ComparisonToOpengl(desc.comparisonFunc));
			}
			else
				glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);

			// min level of detail
			glTexParameterf(target, GL_TEXTURE_MIN_LOD, desc.minLod);
			// max level of detail
			glTexParameterf(target, GL_TEXTURE_MAX_LOD, desc.maxLod);
			// mip level of detail bias
			glTexParameterf(target, GL_TEXTURE_LOD_BIAS, desc.MipLodBias);
			// border color
			glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR,desc.borderColor);
		}
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples , bool flip, bool gammaCorrection)
		:mipLevel(mipLevel),samples(samples),flip(flip),gammaCorrection(gammaCorrection), smpdesc(desc)
	{
		// Immutable
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
		Float16* float16Array = nullptr;
		GLenum Internalformat=0, format=0,dataFormat=0;
		{
			if (extension != "hdr")
			{
				switch (channels)
				{
				case 4:
					Internalformat = gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8;
					format = GL_RGBA;
					dataFormat = GL_UNSIGNED_BYTE;
					break;
				case 3:
					Internalformat = gammaCorrection ? GL_SRGB8 : GL_RGB8;
					format = GL_RGB;
					dataFormat = GL_UNSIGNED_BYTE;
					break;
				case 1:
					Internalformat = GL_R8;
					format = GL_RED;
					dataFormat = GL_UNSIGNED_BYTE;
					break;
				}		
			}
			else
			{
				float16Array = new Float16[width * height * 3];
				for (unsigned int i = 0;i< width * height * 3; i++)
				{
					float16Array[i] = ((float*)pData)[i];
				}

				Internalformat = GL_RGB16F;
				format = GL_RGB;
				dataFormat = GL_HALF_FLOAT;
			}
			
		}
		m_InternalFormat = Internalformat;
		m_Format = format;
		m_textureType = TextureType::Texture2D;
		m_textureFormat = Utils::GetTextureFormatFromGL(m_InternalFormat);
		m_DataFormat = dataFormat;

		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Type  = m_textureType;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->mipLevel = mipLevel;

		QCAT_CORE_ASSERT(Internalformat & format, "Format is not supported!");

		// Texture Create
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		glBindTexture(GL_TEXTURE_2D, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_2D, desc);

		bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);
		
		// Upload Texture
		if(float16Array == nullptr)
			glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, m_DataFormat, pData);
		else
			glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, m_DataFormat, float16Array);
		//glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, m_DataFormat, pData);
		//if (mipLevel > 1 && desc.MIP != Filtering::NONE)
		//	glGenerateTextureMipmap(m_renderID);
		//if (mipLevel > 1 && desc.MIP != Filtering::NONE)
			//glGenerateTextureMipmap(m_renderID);
		// free loaded image
		if (float16Array != nullptr)
			delete[] float16Array;
		stbi_image_free(pData);

		//sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
		:m_width(width),m_height(height), mipLevel(mipLevel), samples(samples), smpdesc(desc)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);

		m_textureFormat = format;
		m_textureType = TextureType::Texture2D;

		m_InternalFormat = Utils::GetTextureInternalFormat(format);
		m_Format = Utils::GetTextureFormat(format);
		m_DataFormat = Utils::GetTextureDataFormat(format);


		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Type = m_textureType;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->mipLevel = mipLevel;
		// for immutable
		/*bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);

		
		if(pData!=nullptr)
			glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, dataFormat,pData);*/

		glBindTexture(GL_TEXTURE_2D, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_2D, desc);

		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D, samples, m_InternalFormat, m_width, m_height, true);
		}
		unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, pData);

		/*for (int i = 0; i < mipLevel; ++i)
		{
			glTexImage2D(GL_TEXTURE_2D, i, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, pData);
			if (temp_Width == 1 || temp_Height == 1 || mipLevel == 1)
				break;
			temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
			temp_Height = std::max<unsigned int>(1, (temp_Height / 2));
		}*/
		//if (mipLevel > 1 && desc.MIP != Filtering::NONE)
		//	glGenerateTextureMipmap(m_renderID);
		//sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::OpenGLTexture2D(Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel,unsigned int samples)
		:m_width(width),m_height(height),mipLevel(mipLevel),samples(samples), smpdesc(desc)
	{
		QCAT_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8, m_Format = GL_RGBA;
		m_textureType = TextureType::Texture2D;
		m_textureFormat = Utils::GetTextureFormatFromGL(m_InternalFormat);
		m_DataFormat = GL_UNSIGNED_BYTE;

		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Type = m_textureType;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = samples;
		this->mipLevel = mipLevel;
		// Texture Create
		/*glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);

		bool multisampled = samples > 1;
		if (multisampled)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, mipLevel, m_InternalFormat, m_width, m_height);*/

		glBindTexture(GL_TEXTURE_2D, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_2D, desc);

		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		}
		else
		{
			unsigned int temp_Width = m_width;
			unsigned int temp_Height = m_height;
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, nullptr);

			/*for (int i = 0; i < mipLevel; ++i)
			{
				glTexImage2D(GL_TEXTURE_2D, i, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, nullptr);
				if (temp_Width == 1 || temp_Height == 1 || mipLevel == 1)
					break;
				temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
				temp_Height = std::max<unsigned int>(1, (temp_Height / 2));
			}*/
		}
		//if (mipLevel > 1 && desc.MIP != Filtering::NONE)
		//	glGenerateTextureMipmap(m_renderID);
		//sampler = OpenGLSampler::Create(desc);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::GenerateMipMap()
	{
		if (mipLevel > 1 && smpdesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLTexture2D::SetData(void* pData, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = m_Format == GL_RGBA ? 4 : 3;
		QCAT_CORE_ASSERT(size == m_width * m_height * bpc,"Data must be entire texture!");
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, GL_UNSIGNED_BYTE, pData);
	}
	void OpenGLTexture2D::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		glBindTexture(GL_TEXTURE_2D, m_renderID);
		desc.Width = width;
		desc.Height = height;
		m_width = width;
		m_height = height;
		unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;
		for (int i = 0; i < mipLevel; ++i)
		{
			glTexImage2D(GL_TEXTURE_2D, i, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, nullptr);
			if (temp_Width == 1 || temp_Height == 1 || mipLevel == 1)
				break;
			temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
			temp_Height = std::max<unsigned int>(1, (temp_Height / 2));
		}
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		// 1st parameter is for slot
		//glActiveTexture(GL_TEXTURE0 + slot);
		//glBindTexture(GL_TEXTURE_2D, m_renderID);
		glBindTextureUnit(slot, m_renderID);
		//sampler->Bind(slot);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels,bool flip , bool gammaCorrection)
		:flip(flip),gammaCorrection(gammaCorrection), smpdesc(desc)
	{
		QCAT_PROFILE_FUNCTION();

		uint32_t textureID;
		glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_CUBE_MAP, desc);

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
			m_textureType = TextureType::TextureCube;
			m_textureFormat = Utils::GetTextureFormatFromGL(imageformat);

			QCAT_CORE_ASSERT(data, "Failed to load Image!");
			m_width = width;
			m_height = height;
			m_InternalFormat = imageformat;
			m_Format = format;

			this->desc.Width = m_width;
			this->desc.Height = m_height;
			this->desc.Type = m_textureType;
			this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
			this->desc.usage = TextureUsage::Immutable;
			this->mipLevel = mipLevel;

			for(int j = 0; j < mipLevels; ++j)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, m_InternalFormat, width, height, 0, m_Format, GL_UNSIGNED_BYTE, data);
			}
			stbi_image_free(data);
		}
		//if (mipLevels > 1 && desc.MIP != Filtering::NONE)
		//	glGenerateTextureMipmap(m_renderID);

		//sampler = OpenGLSampler::Create(desc);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevels, void* pData)
		:m_width(width),m_height(height), mipLevel(mipLevels), smpdesc(desc)
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_renderID);

		//glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_CUBE_MAP, desc);

		m_textureType = TextureType::TextureCube;
		m_textureFormat = format;

		GLenum internalFormat= Utils::GetTextureInternalFormat(format);
		GLenum textureformat = Utils::GetTextureFormat(format);
		GLenum dataFormat = Utils::GetTextureDataFormat(format);
		m_InternalFormat = internalFormat;
		m_Format = textureformat;
		m_DataFormat = dataFormat;

		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Type = m_textureType;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->mipLevel = mipLevel;

		unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;

		/*for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < mipLevels; ++j)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,j, m_InternalFormat, width, height, 0, m_Format, m_DataFormat, pData);
				if (temp_Width == 1 || temp_Height == 1 || mipLevels==1)
					break;
				temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
				temp_Height = std::max<unsigned int>(1, (temp_Height / 2));	
			}
		}*/
		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, width, height, 0, m_Format, m_DataFormat, pData);
		}
		// immutable
		//glTextureStorage2D(m_renderID, mipLevels, internalFormat, m_width, m_height);

		//if(mipLevels>1 && desc.MIP != Filtering::NONE)
		//  glGenerateTextureMipmap(m_renderID);
		
		//sampler = OpenGLSampler::Create(desc);
	}
	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLCubeMapTexture::GenerateMipMap()
	{
		if (mipLevel > 1 && smpdesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLCubeMapTexture::SetData(void* pData, unsigned int size)
	{
	}
	void OpenGLCubeMapTexture::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		desc.Width = width;
		desc.Height = height;
		m_width = width;
		m_height = height;
		unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < mipLevel; ++j)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, nullptr);
				if (temp_Width == 1 || temp_Height == 1 || mipLevel == 1)
					break;
				temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
				temp_Height = std::max<unsigned int>(1, (temp_Height / 2));
			}
		}
	}
	void OpenGLCubeMapTexture::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();

		// 1st parameter is for slot
		glBindTextureUnit(slot, m_renderID);
		//glActiveTexture(GL_TEXTURE0+slot);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		//sampler->Bind(slot);
	}
	
}