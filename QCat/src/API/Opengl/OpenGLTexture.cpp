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
		:m_mipLevel(mipLevel), m_samples(samples),flip(flip),gammaCorrection(gammaCorrection), smpdesc(desc)
	{
		// Immutable
		QCAT_PROFILE_FUNCTION();
		this->path = path;
		auto begin = path.find_last_of('.');
		std::string extension = path.substr(begin + 1, path.length());
		int width, height, channels;
		if(!flip)
			stbi_set_flip_vertically_on_load(1);
		else
			stbi_set_flip_vertically_on_load(0);
		void* stb_data = nullptr;
		void *pData=nullptr;
		{
			QCAT_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			if (extension != "hdr")
			{
				stb_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load Image!");
			}
			else
			{
				stb_data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
				QCAT_CORE_ASSERT(stb_data, "Failed to load HDR image!");
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
				case 2:
					Internalformat = GL_RG8;
					format = GL_RG;
					dataFormat = GL_UNSIGNED_BYTE;
					break;
				case 1:
					Internalformat = GL_R8;
					format = GL_RED;
					dataFormat = GL_UNSIGNED_BYTE;
					break;
				}	
				pData = stb_data;
			}
			else
			{
				float16Array = new Float16[width * height * 3];
				for (unsigned int i = 0;i< width * height * 3; i++)
				{
					float16Array[i] = ((float*)stb_data)[i];
				}

				Internalformat = GL_RGB16F;
				format = GL_RGB;
				dataFormat = GL_HALF_FLOAT;
				pData = float16Array;
			}
			
		}
		m_InternalFormat = Internalformat;
		m_Format = format;
		m_DataFormat = dataFormat;

		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.Type = TextureType::Texture2D;
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;

		QCAT_CORE_ASSERT(Internalformat & format, "Format is not supported!");
		Validate(m_Format, m_InternalFormat, m_DataFormat, m_width, m_height, m_mipLevel, m_samples, pData);

		// free loaded image
		if (float16Array != nullptr)
			delete[] float16Array;
		stbi_image_free(stb_data);
	}
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
		:m_width(width),m_height(height), m_mipLevel(mipLevel), m_samples(samples), smpdesc(desc)
	{
		m_InternalFormat = Utils::GetTextureInternalFormat(format);
		m_Format = Utils::GetTextureFormat(format);
		m_DataFormat = Utils::GetTextureDataFormat(format);

		this->desc.Format = format;
		this->desc.Type = TextureType::Texture2D;
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->desc.SampleCount = m_samples;
		this->desc.MipLevels= m_mipLevel;

		Validate(m_Format, m_InternalFormat, m_DataFormat, m_width, m_height, m_mipLevel, m_samples, pData);


	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::GenerateMipMap()
	{
		if (m_mipLevel > 1 && smpdesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLTexture2D::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = Utils::GetTextureComponentCount(desc.Format);
		QCAT_CORE_ASSERT(size == m_width * m_height * bpc,"Data must be entire texture!");
		glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, Utils::GetTextureDataFormat(desc.Format), data);
	}
	void OpenGLTexture2D::GetData(void* data, uint32_t mipLevel, uint32_t textureindex )
	{
		glGetTexImage(Utils::GetTextureTarget(desc.Type,desc.SampleCount>1), mipLevel, Utils::GetTextureFormat(desc.Format), Utils::GetTextureDataFormat(desc.Format), data);
	}
	void OpenGLTexture2D::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		glBindTexture(GL_TEXTURE_2D, m_renderID);
		desc.Width = width;
		desc.Height = height;
		m_width = width;
		m_height = height;
		/*unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;
		for (int i = 0; i < m_mipLevel; ++i)
		{
			glTexImage2D(GL_TEXTURE_2D, i, m_InternalFormat, temp_Width, temp_Height,0 , m_Format, m_DataFormat, nullptr);
			if (temp_Width == 1 || temp_Height == 1 || m_mipLevel == 1)
				break;
			temp_Width = std::max<unsigned int>(1, (temp_Width / 2));
			temp_Height = std::max<unsigned int>(1, (temp_Height / 2));
		}*/

		Validate(m_Format, m_InternalFormat, m_DataFormat, m_width, m_height, desc.MipLevels, desc.SampleCount, nullptr);
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		// 1st parameter is for slot
		//glActiveTexture(GL_TEXTURE0 + slot);
		//glBindTexture(GL_TEXTURE_2D, m_renderID);
		glBindTextureUnit(slot, m_renderID);
	}
	void OpenGLTexture2D::Validate(GLenum format, GLenum internalFormat, GLenum dataFormat, uint32_t width, uint32_t heigth, uint32_t mipLevels, uint32_t samples, void* pData)
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
		}
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		glBindTexture(GL_TEXTURE_2D, m_renderID);
		Utils::SetTextureParameter(GL_TEXTURE_2D, smpdesc);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (samples > 1)
			glTextureStorage2DMultisample(m_renderID, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, m_mipLevel, m_InternalFormat, m_width, m_height);
		if (pData != nullptr)
			glTextureSubImage2D(m_renderID, 0, 0, 0, m_width, m_height, m_Format, dataFormat, pData);
		//sampler = OpenGLSampler::Create(smpdesc);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels,bool flip , bool gammaCorrection)
		:flip(flip),gammaCorrection(gammaCorrection), smpdesc(desc)
	{
		QCAT_PROFILE_FUNCTION();

		if (!flip)
			stbi_set_flip_vertically_on_load(0);
		else
			stbi_set_flip_vertically_on_load(1);
		int width, height, channels;
		
		Validate(GL_TEXTURE_CUBE_MAP, 0, m_InternalFormat, width, height, 0, m_Format, m_DataFormat);
		Utils::SetTextureParameter(GL_TEXTURE_CUBE_MAP, desc);

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
			m_InternalFormat = imageformat;
			m_Format = format;
			this->desc.Format = Utils::GetTextureFormatFromGL(imageformat);
			this->desc.Type = TextureType::TextureCube;
			this->desc.Width = m_width;
			this->desc.Height = m_height;
			this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
			this->desc.usage = TextureUsage::Immutable;
			this->desc.MipLevels = m_mipLevel;

			SetData(data, width * height * channels, (uint32_t)TextureType::TextureCube_PositiveX + i);
			stbi_image_free(data);
		}

	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevels, void* pData)
		:m_width(width),m_height(height), m_mipLevel(mipLevels), smpdesc(desc)
	{
		GLenum internalFormat= Utils::GetTextureInternalFormat(format);
		GLenum textureformat = Utils::GetTextureFormat(format);
		GLenum dataFormat = Utils::GetTextureDataFormat(format);
		m_InternalFormat = internalFormat;
		m_Format = textureformat;
		m_DataFormat = dataFormat;

		this->desc.Format = format;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->desc.MipLevels = m_mipLevel;
		this->desc.SampleCount = 0;
		unsigned int temp_Width = m_width;
		unsigned int temp_Height = m_height;

		Validate(GL_TEXTURE_CUBE_MAP, 0, m_InternalFormat, width, height, 0, m_Format, m_DataFormat);
		Utils::SetTextureParameter(GL_TEXTURE_CUBE_MAP, desc);
		//for (int i = 0; i < 6; ++i)
		//{
		//	Validate(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, width, height, 0, m_Format, m_DataFormat, pData);
		//}
	}
	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLCubeMapTexture::Validate(GLenum target, GLenum format, GLenum internalFormat, GLenum dataFormat, uint32_t width, uint32_t heigth, uint32_t mipLevels, uint32_t samples)
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
		}
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_renderID);;
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderID);
		if (desc.SampleCount > 1)
			glTexStorage2DMultisample(target, samples, m_InternalFormat, m_width, m_height, true);
		else
			glTexStorage2D(target, m_mipLevel, m_InternalFormat, m_width, m_height);
	}
	void OpenGLCubeMapTexture::GenerateMipMap()
	{
		if (m_mipLevel > 1 && smpdesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLCubeMapTexture::SetData(void* data, unsigned int size, uint32_t textureindex )
	{
		QCAT_PROFILE_FUNCTION();

		if (textureindex > 3)
		{
			TextureType tempType = static_cast<TextureType>(textureindex);
			if (tempType == TextureType::TextureCube)
				tempType = TextureType::TextureCube_PositiveX;

			unsigned int bpc = Utils::GetTextureComponentCount(desc.Format);
			QCAT_CORE_ASSERT(size == desc.Width * desc.Height * bpc, "Data must be entire texture!");

			glTexSubImage2D(Utils::GetTextureTarget(tempType, false), 0, 0, 0, desc.Width, desc.Height, m_Format, m_DataFormat, data);
		}
		else
			QCAT_CORE_ERROR("Texture SetData Error : CubeMapTexture Index Wrong!");

	}
	void OpenGLCubeMapTexture::GetData(void* data, uint32_t mipLevel, uint32_t textureindex )
	{
		if (textureindex > 3)
		{
			TextureType tempType = static_cast<TextureType>(textureindex);
			if (tempType == TextureType::TextureCube)
				tempType = TextureType::TextureCube_PositiveX;
			glGetTexImage(Utils::GetTextureTarget(tempType, desc.SampleCount > 1), mipLevel, Utils::GetTextureFormat(desc.Format), Utils::GetTextureDataFormat(desc.Format), data);
		}
		else
			QCAT_CORE_ERROR("Texture GetData Error : CubeMapTexture Index Wrong!");
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
			for (int j = 0; j < m_mipLevel; ++j)
			{
				glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, m_InternalFormat, temp_Width, temp_Height, 0, m_Format, m_DataFormat, nullptr);
				if (temp_Width == 1 || temp_Height == 1 || m_mipLevel == 1)
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
	}
	
	//Texture Utility
	OpenGLTextureUtility::OpenGLTextureUtility()
	{
		glCreateFramebuffers(1, &m_RendererID);
	}

	OpenGLTextureUtility::~OpenGLTextureUtility()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLTextureUtility::CopyTexture2D_(Ref<Texture>& srcTex, Ref<Texture>& dstTex, uint32_t mipLevel, QCAT_BOX boxregion)
	{
		if (srcTex->GetTextureType() == TextureType::Texture2D && dstTex->GetTextureType() == TextureType::Texture2D)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)srcTex->GetTexture(), mipLevel);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glCopyTextureSubImage2D((GLuint)dstTex->GetTexture(), mipLevel, boxregion.xoffset, boxregion.yoffset, boxregion.x, boxregion.y, boxregion.width, boxregion.height);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else
		{
			QCAT_CORE_ERROR("CopyTexture2D Error : TextureType is wrong !");
		}
	}

	void OpenGLTextureUtility::CopyCubemapFace2D_(Ref<Texture>& srcCubeMap, Ref<Texture>& dstTex, uint32_t index, uint32_t mipLevel, QCAT_BOX boxregion)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		TextureType type = TextureType(index);
		GLenum cubeFace;
		switch (type)
		{
		case TextureType::TextureCube_PositiveX:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;	break;
		case TextureType::TextureCube_NegativeX:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;	break;
		case TextureType::TextureCube_PositiveY:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;	break;
		case TextureType::TextureCube_NegativeY:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;	break;
		case TextureType::TextureCube_PositiveZ:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;	break;
		case TextureType::TextureCube_NegativeZ:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;	break;
		default:
			cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			QCAT_CORE_ERROR("CopyCubemapFace2D Error : Wrong CubeMapIndex!");
		}
		GLenum AttachmentType1 = Utils::GetAttachmentTypeForCopy(srcCubeMap->GetTextureFormat());
		GLenum AttachmentType2 = Utils::GetAttachmentTypeForCopy(dstTex->GetTextureFormat());
		if (srcCubeMap->GetTextureType() == TextureType::TextureCube && dstTex->GetTextureType() == TextureType::Texture2D)
		{
			
			if (AttachmentType1 == AttachmentType2)
			{
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, AttachmentType1, cubeFace, (GLuint)srcCubeMap->GetTexture(), mipLevel);
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is bad");
				glCopyTextureSubImage2D((GLuint)dstTex->GetTexture(), mipLevel, boxregion.xoffset, boxregion.yoffset, boxregion.x, boxregion.y, boxregion.width, boxregion.height);
			}
			else
				QCAT_CORE_ERROR("CopyCubemapFace2D Error : Both AttachmentTypes are diffrent!");
		}
		else
			QCAT_CORE_ERROR("CopyCubemapFace2D Error : TextureType is wrong !");

		glFramebufferTexture(GL_FRAMEBUFFER, AttachmentType1, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}