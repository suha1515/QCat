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
		void SetTextureParameter(GLuint texture, Sampler_Desc desc)
		{
			Utils::SetTextureFilter(texture, GL_TEXTURE_MIN_FILTER, desc.MIN, desc.MIP);
			Utils::SetTextureFilter(texture, GL_TEXTURE_MAG_FILTER, desc.MAG);
			Utils::SetTextureWrapMethod(texture, GL_TEXTURE_WRAP_S, desc.addressU);
			Utils::SetTextureWrapMethod(texture, GL_TEXTURE_WRAP_T, desc.addressV);
			Utils::SetTextureWrapMethod(texture, GL_TEXTURE_WRAP_R, desc.addressW);

			if (desc.mode == FilterMode::COMPARISON)
			{
				glTextureParameteri(texture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTextureParameteri(texture, GL_TEXTURE_COMPARE_FUNC, Utils::ComparisonToOpengl(desc.comparisonFunc));
			}
			else
				glTextureParameteri(texture, GL_TEXTURE_COMPARE_MODE, GL_NONE);

			// min level of detail
			glTextureParameterf(texture, GL_TEXTURE_MIN_LOD, desc.minLod);
			// max level of detail
			glTextureParameterf(texture, GL_TEXTURE_MAX_LOD, desc.maxLod);
			// mip level of detail bias
			glTextureParameterf(texture, GL_TEXTURE_LOD_BIAS, desc.MipLodBias);
			// border color
			glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR,desc.borderColor);
		}

		void* LoadImageFromFile(const std::string& path,bool gammaCorrection, bool flip,uint32_t& width,uint32_t& height,uint32_t& channels, GLenum& format, GLenum& internalformat, GLenum& dataformat)
		{
			auto begin = path.find_last_of('.');
			std::string extension = path.substr(begin + 1, path.length());
			int width_=0, height_=0, channels_=0;
			if (!flip)
				stbi_set_flip_vertically_on_load(1);
			else
				stbi_set_flip_vertically_on_load(0);
			void* pData = nullptr;
			void* stb_data = nullptr;
			{
				QCAT_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
				if (extension != "hdr")
				{
					stb_data = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
					QCAT_CORE_ASSERT(stb_data, "Failed to load Image!");
				}
				else
				{
					//stb_data = stbi_load_16(path.c_str(), &width_, &height_, &channels, 0);
					stb_data = stbi_loadf(path.c_str(), &width_, &height_, &channels_, 0);
					QCAT_CORE_ASSERT(stb_data, "Failed to load HDR image!");
				}
			}
			width = width_;
			height = height_;
			channels = channels_;
			Float16* float16Array = nullptr;
			{
				if (extension != "hdr")
				{
					switch (channels)
					{
					case 4:
						internalformat = gammaCorrection ? GL_SRGB8_ALPHA8 : GL_RGBA8;
						format = GL_RGBA;
						dataformat = GL_UNSIGNED_BYTE;

						break;
					case 3:
						internalformat = gammaCorrection ? GL_SRGB8 : GL_RGB8;
						format = GL_RGB;
						dataformat = GL_UNSIGNED_BYTE;
						break;
					case 2:
						internalformat = GL_RG8;
						format = GL_RG;
						dataformat = GL_UNSIGNED_BYTE;
						break;
					case 1:
						internalformat = GL_R8;
						format = GL_RED;
						dataformat = GL_UNSIGNED_BYTE;
						break;
					}
					pData = stb_data;
				}
				else
				{
					float16Array = new Float16[width * height * 3];
					for (unsigned int i = 0; i < width * height * 3; i++)
					{
						float16Array[i] = ((float*)stb_data)[i];
					}

					internalformat = GL_RGB16F;
					format = GL_RGB;
					dataformat = GL_HALF_FLOAT;
					pData = float16Array;
				}
			}
			return pData;
		}
	}
	OpenGLTexture2D::OpenGLTexture2D(Texture_Desc desc, Sampler_Desc sampDesc, void* pData)
	{
		this->desc = desc;
		this->sampDesc = sampDesc;

		m_InternalFormat = Utils::GetTextureInternalFormat(desc.Format);
		m_Format = Utils::GetTextureFormat(desc.Format);
		m_DataFormat = Utils::GetTextureDataFromFormat(desc.Format);

		Validate(pData);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples , bool flip, bool gammaCorrection)
	{
		// Immutable
		QCAT_PROFILE_FUNCTION();
		this->sampDesc = desc;
		this->pathes.push_back(path);
		uint32_t width = 0, height = 0, channels=0;
		void* pData = Utils::LoadImageFromFile(path, gammaCorrection, flip, width, height, channels, m_Format, m_InternalFormat, m_DataFormat);

		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.Type = TextureType::Texture2D;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;

		QCAT_CORE_ASSERT(m_InternalFormat & m_Format, "Format is not supported!");
		Validate(pData);
		if (pData != nullptr)
			free(pData);
	}
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, Sampler_Desc samplerDesc, unsigned int width, unsigned int height, unsigned int mipLevel, unsigned int samples, void* pData)
	{
		this->pathes.push_back("none");
		this->sampDesc = samplerDesc;

		this->desc.Format = format;
		this->desc.Type = TextureType::Texture2D;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;

		m_InternalFormat = Utils::GetTextureInternalFormat(this->desc.Format);
		m_Format = Utils::GetTextureFormat(this->desc.Format);
		m_DataFormat = Utils::GetTextureDataFromFormat(this->desc.Format);

		Validate(pData);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTexture2D::GenerateMipMap()
	{
		if (desc.MipLevels > 1 && sampDesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLTexture2D::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
		QCAT_PROFILE_FUNCTION();

		unsigned int bpc = Utils::GetTextureComponentCount(desc.Format);
		QCAT_CORE_ASSERT(size == desc.Width * desc.Height* bpc,"Data must be entire texture!");
		glTextureSubImage2D(m_renderID, 0, 0, 0, desc.Width, desc.Height, m_Format, Utils::GetTextureDataFromFormat(desc.Format), data);
	}
	void OpenGLTexture2D::GetData(void* data, uint32_t mipLevel, uint32_t textureindex )
	{
		glGetTexImage(Utils::GetTextureTarget(desc.Type,desc.SampleCount>1), mipLevel, Utils::GetTextureFormat(desc.Format), Utils::GetTextureDataFromFormat(desc.Format), data);
	}
	void OpenGLTexture2D::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
		glBindTexture(GL_TEXTURE_2D, m_renderID);
		desc.Width = width;
		desc.Height = height;
		Validate(nullptr);
	}
	void OpenGLTexture2D::ReadData(uint32_t miplevel,uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, uint32_t bufSize, void* pixels)
	{
		GLenum glFormat = Utils::GetTextureFormat(format);
		GLenum glDataType = Utils::GetTextureDataType(dataType);
		uint32_t totalsize = width * height * Utils::GetDataSize(dataType);
		if (totalsize > bufSize)
			QCAT_CORE_ERROR("return BufSize is {0} and requited bufSize {1} : Wrong Buffer Size!", totalsize, bufSize);

		glGetTextureSubImage(m_renderID, miplevel, xoffset, yoffset, 0, width, height,1, glFormat, glDataType, bufSize, pixels);
	}
	void OpenGLTexture2D::ClearData(uint32_t miplevel, uint32_t xoffset, uint32_t yoffset, uint32_t width, uint32_t height, TextureFormat format, TextureDataType dataType, const void* data)
	{
		GLenum glFormat = Utils::GetTextureFormat(format);
		GLenum glDataType = Utils::GetTextureDataType(dataType);

		glClearTexSubImage(m_renderID, miplevel, xoffset, yoffset, 0, width, height, 1, glFormat, glDataType, data);
		//glClearTexImage(m_renderID, 0, glFormat, glDataType, data);
	}
	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_renderID);
	}
	void OpenGLTexture2D::Validate(void* pData)
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
			m_renderID = 0;
		}
		glCreateTextures(GL_TEXTURE_2D, 1, &m_renderID);
		Utils::SetTextureParameter(m_renderID, sampDesc);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (desc.SampleCount > 1)
			glTextureStorage2DMultisample(m_renderID, desc.SampleCount, m_InternalFormat, desc.Width, desc.Height, true);
		else
			glTextureStorage2D(m_renderID, desc.MipLevels, m_InternalFormat, desc.Width, desc.Height);
		if (pData != nullptr)
			glTextureSubImage2D(m_renderID, 0, 0, 0, desc.Width, desc.Height, m_Format, m_DataFormat, pData);

		sampler = SamplerState::Create(sampDesc);
	}
	OpenGlTexture2DArray::OpenGlTexture2DArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel, unsigned int samples)
	{
		// Immutable
		QCAT_PROFILE_FUNCTION();		

		this->sampDesc = desc;
		m_InternalFormat = Utils::GetTextureInternalFormat(format);
		m_Format = Utils::GetTextureFormat(format);
		m_DataFormat = Utils::GetTextureDataFromFormat(format);

		this->desc.Format = format;
		this->desc.Type = TextureType::Texture2DArray;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.ArraySize = depth;
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;

		QCAT_CORE_ASSERT(m_InternalFormat & m_Format, "Format is not supported!");
		Validate();
	}
	OpenGlTexture2DArray::OpenGlTexture2DArray(std::vector<std::string> imagePath, Sampler_Desc desc, unsigned int mipLevel, unsigned int samples, bool flip, bool gamacorrection)
	{
		// Immutable
		QCAT_PROFILE_FUNCTION();
		this->sampDesc = desc;

		this->pathes = imagePath;
		uint32_t width = 0, height = 0, channels = 0;
		for (auto path : imagePath)
		{
			void* pData = Utils::LoadImageFromFile(path, gamacorrection, flip, width, height, channels, m_Format, m_InternalFormat, m_DataFormat);
			Validate(pData);
			if (pData != nullptr)
				free(pData);
		}
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.Type = TextureType::Texture2DArray;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.Depth = imagePath.size();
		this->desc.usage = TextureUsage::Immutable;
		this->desc.SampleCount = samples;
		this->desc.MipLevels = mipLevel;

		QCAT_CORE_ASSERT(m_InternalFormat & m_Format, "Format is not supported!");
		
	}
	OpenGlTexture2DArray::~OpenGlTexture2DArray()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGlTexture2DArray::GenerateMipMap()
	{
		if (desc.MipLevels > 1 && sampDesc.MIP != Filtering::NONE)
			glGenerateTextureMipmap(m_renderID);
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGlTexture2DArray::SetData(void* data, unsigned int size, uint32_t textureindex)
	{
	}
	void OpenGlTexture2DArray::GetData(void* data, uint32_t mipLevel, uint32_t textureindex)
	{
	}
	void OpenGlTexture2DArray::SetSize(uint32_t width, uint32_t height, uint32_t depth)
	{
	}
	void OpenGlTexture2DArray::Bind(unsigned int slot) const
	{
		QCAT_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_renderID);
	}
	void OpenGlTexture2DArray::Validate(void* pData, uint32_t index)
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
			m_renderID = 0;
		}
		GLuint error;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_renderID);
		Utils::SetTextureParameter(m_renderID, sampDesc);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (desc.SampleCount > 1)
			glTextureStorage3DMultisample(m_renderID, desc.SampleCount, m_InternalFormat, desc.Width, desc.Height,desc.Depth, true);
		else
			glTextureStorage3D(m_renderID, desc.MipLevels, m_InternalFormat, desc.Width, desc.Height,desc.ArraySize);
		error = glGetError();
		if (pData != nullptr && index < desc.Depth)
			glTextureSubImage3D(m_renderID, 0, 0, 0,0, desc.Width, desc.Height, index, m_Format, m_DataFormat, pData);

		sampler = SamplerState::Create(sampDesc);
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(const std::vector<std::string> imgPathes, Sampler_Desc desc, unsigned int mipLevels,bool flip , bool gammaCorrection)
		:flip(flip),gammaCorrection(gammaCorrection)
	{
		QCAT_PROFILE_FUNCTION();
		this->sampDesc = desc;
		uint32_t width=0, height=0, channels=0;	
		
		Utils::SetTextureParameter(m_renderID, desc);
	
		void*pData =  Utils::LoadImageFromFile(imgPathes[0].c_str(), gammaCorrection, flip, width, height, channels, m_Format, m_InternalFormat, m_DataFormat);
		Validate();
		SetData(pData, width * height * channels, (uint32_t)TextureCubeFace::TextureCube_PositiveX );
		if (pData != nullptr)
		{
			free(pData);
			pData = nullptr;
		}
		m_width = width;
		m_height = height;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_Format);
		this->desc.Type = TextureType::TextureCube;
		this->desc.Width = m_width;
		this->desc.Height = m_height;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Immutable;
		this->desc.MipLevels = m_mipLevel;

		for (uint32_t i = 1; i < imgPathes.size(); ++i)
		{
			GLenum format, internalformat, dataformat;
			
			pData = Utils::LoadImageFromFile(imgPathes[i].c_str(), gammaCorrection, flip, width, height, channels, format,internalformat,dataformat);
			if (format != m_Format || internalformat != m_InternalFormat || dataformat != m_DataFormat)
			{
				QCAT_CORE_ASSERT(false, "Failed to load Image!");
				break;
			}
			SetData(pData, width * height * channels, (uint32_t)TextureCubeFace::TextureCube_PositiveX + i);
			if (pData != nullptr)
			{
				free(pData);
				pData = nullptr;
			}
		}
	}
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int mipLevels)
		:m_width(width),m_height(height), m_mipLevel(mipLevels)
	{
		this->sampDesc = desc;
		GLenum internalFormat= Utils::GetTextureInternalFormat(format);
		GLenum textureformat = Utils::GetTextureFormat(format);
		GLenum dataFormat = Utils::GetTextureDataFromFormat(format);
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

		Validate();
		Utils::SetTextureParameter(m_renderID, desc);
	}
	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLCubeMapTexture::Validate()
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
			m_renderID = 0;
		}
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_renderID);;
		if (desc.SampleCount > 1)
			glTexStorage2DMultisample(GL_TEXTURE_CUBE_MAP, desc.SampleCount, m_InternalFormat, m_width, m_height, true);
		else
			glTextureStorage2D(m_renderID, m_mipLevel, m_InternalFormat, m_width, m_height);		

		//if (m_mipLevel > 1 && smpdesc.MIP != Filtering::NONE)
		//{
		//	glGenerateTextureMipmap(m_renderID);
		//}

		sampler = SamplerState::Create(sampDesc);
	}
	void OpenGLCubeMapTexture::GenerateMipMap()
	{
		if (m_mipLevel > 1 && sampDesc.MIP != Filtering::NONE)
		{
			glGenerateTextureMipmap(m_renderID);
		}
		else
			QCAT_CORE_ERROR("this texture can't generate mipmap!");
	}
	void OpenGLCubeMapTexture::SetData(void* data, unsigned int size, uint32_t textureindex )
	{
		QCAT_PROFILE_FUNCTION();

		if (textureindex < 6)
		{
			TextureCubeFace tempType = static_cast<TextureCubeFace>(textureindex);

			unsigned int bpc = Utils::GetTextureComponentCount(desc.Format);
			QCAT_CORE_ASSERT(size == desc.Width * desc.Height * bpc, "Data must be entire texture!");
			//glTexSubImage2D(Utils::GetTextureTargetFromCube(tempType, false), 0, 0, 0, desc.Width, desc.Height, m_Format, m_DataFormat, data);
			//glTextureSubImage2D(m_renderID, 0, 0, 0, desc.Width, desc.Height, m_Format, m_DataFormat, data);
			glTextureSubImage3D(m_renderID, 0, 0, 0,0, desc.Width, desc.Height,textureindex, m_Format, m_DataFormat, data);

		}
		else
			QCAT_CORE_ERROR("Texture SetData Error : CubeMapTexture Index Wrong!");

	}
	void OpenGLCubeMapTexture::GetData(void* data, uint32_t mipLevel, uint32_t textureindex )
	{
		if (textureindex < 6)
		{
			TextureCubeFace tempType = static_cast<TextureCubeFace>(textureindex);
			glGetTexImage(Utils::GetTextureTargetFromCube(tempType, desc.SampleCount > 1), mipLevel, Utils::GetTextureFormat(desc.Format), Utils::GetTextureDataFromFormat(desc.Format), data);
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
	OpenGLCubeMapArray::OpenGLCubeMapArray(TextureFormat format, Sampler_Desc desc, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipLevel, unsigned int samples)
	{
		GLenum internalFormat = Utils::GetTextureInternalFormat(format);
		GLenum textureformat = Utils::GetTextureFormat(format);
		GLenum dataFormat = Utils::GetTextureDataFromFormat(format);
		m_InternalFormat = internalFormat;
		m_Format = textureformat;
		m_DataFormat = dataFormat;

		this->desc.Format = format;
		this->desc.Type = TextureType::TextureCube;
		this->desc.Width = width;
		this->desc.Height = height;
		this->desc.ArraySize = depth;
		this->desc.Format = Utils::GetTextureFormatFromGL(m_InternalFormat);
		this->desc.usage = TextureUsage::Default;
		this->desc.MipLevels = mipLevel;
		this->desc.SampleCount = samples;

		Validate();
		Utils::SetTextureParameter(m_renderID, desc);
	}

	OpenGLCubeMapArray::~OpenGLCubeMapArray()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLCubeMapArray::Validate()
	{
		if (m_renderID > 0)
		{
			glDeleteTextures(1, &m_renderID);
			m_renderID = 0;
		}
		glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_renderID);;
		if (desc.SampleCount > 1)
			glTexStorage3DMultisample(m_renderID, desc.SampleCount, m_InternalFormat, desc.Width, desc.Height,desc.ArraySize*6, true);
		else
			glTextureStorage3D(m_renderID, desc.MipLevels, m_InternalFormat, desc.Width, desc.Height,desc.ArraySize*6);

		sampler = SamplerState::Create(sampDesc);
	}

	void OpenGLCubeMapArray::Bind(unsigned int slot) const
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
		TextureCubeFace type = TextureCubeFace(index);
		GLenum cubeFace;
		switch (type)
		{
		case TextureCubeFace::TextureCube_PositiveX:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;	break;
		case TextureCubeFace::TextureCube_NegativeX:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;	break;
		case TextureCubeFace::TextureCube_PositiveY:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;	break;
		case TextureCubeFace::TextureCube_NegativeY:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;	break;
		case TextureCubeFace::TextureCube_PositiveZ:  cubeFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;	break;
		case TextureCubeFace::TextureCube_NegativeZ:  cubeFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;	break;
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
	OpenGLTextureShaderView::OpenGLTextureShaderView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t numMip, uint32_t startLayer, uint32_t numlayer)
		:m_renderID(0)
	{	
		this->type = ViewType::ShaderResourceView;

		Texture_Desc srcdesc = texture->GetDesc();
		bool multisampled = srcdesc.SampleCount > 1;
		GLenum target = Utils::GetTextureTarget(type, multisampled);
		GLenum internalformat = Utils::GetTextureInternalFormat(format);

		uint32_t bitSize = Utils::GetBitSizeFromFormat(format);
		uint32_t srcBitSize = Utils::GetBitSizeFromFormat(srcdesc.Format);

		//GLenum srcTextureFormat = Utils::GetTextureDataFromFormat(srcdesc.Format);
		uint32_t srcTexMipLevels = srcdesc.MipLevels;
		uint32_t srcTexLayerNum = srcdesc.ArraySize;
		GLuint srcTexID = (GLuint)texture->GetTexture();

		uint32_t viewDimension = Utils::GetDimensionFromType(type);
		uint32_t srcDimension = Utils::GetDimensionFromType(srcdesc.Type);

		glGenTextures(1, &m_renderID);
		switch (srcdesc.Type)
		{
		case TextureType::Texture1D:
		case TextureType::Texture1DArray:
		case TextureType::Texture2D:
		case TextureType::Texture2DArray:
		case TextureType::TextureCube:
		case TextureType::Texture3D:
			glTextureView(m_renderID, target, srcTexID, internalformat, startMip, numMip, startLayer, numlayer);
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for ShaderResourceView!");
			break;
		}

		this->viewDesc = srcdesc;
		this->viewDesc.ArraySize = numlayer;
		this->viewDesc.MipLevels = numMip;
		this->viewDesc.Format = format;
		
		uint32_t temp_width = srcdesc.Width;
		uint32_t temp_height = srcdesc.Height;
		for (int i = 0; i < startMip; ++i)
		{
			if (temp_width == 1 || temp_height == 1)
				break;
			temp_width = std::max<unsigned int>(1, (temp_width / 2));
			temp_height = std::max<unsigned int>(1, (temp_height / 2));
		}
		this->viewDesc.Width = temp_width;
		this->viewDesc.Height = temp_height;
		this->viewDesc.Type = type;
	}
	OpenGLTextureShaderView::~OpenGLTextureShaderView()
	{
		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLTextureShaderView::Bind(uint32_t slot, ShaderType type) const
	{
		glBindTextureUnit(slot, m_renderID);
	}
	OpenGLRenderTargetView::OpenGLRenderTargetView(TextureType type, const Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t startLayer, uint32_t numlayer)
		:m_renderID(0), m_startMip(startMip)
	{
		this->type = ViewType::RenderTargetView;

		Texture_Desc srcdesc = texture->GetDesc();
		bool multisampled = srcdesc.SampleCount > 1;
		GLenum target = Utils::GetTextureTarget(type, multisampled);
		GLenum internalformat = Utils::GetTextureInternalFormat(format);

		uint32_t bitSize = Utils::GetBitSizeFromFormat(format);
		uint32_t srcBitSize = Utils::GetBitSizeFromFormat(srcdesc.Format);

		uint32_t srcTexMipLevels = srcdesc.MipLevels;
		uint32_t srcTexLayerNum = srcdesc.ArraySize;
		GLuint srcTexID = (GLuint)texture->GetTexture();
		GLenum error;
		glGenTextures(1, &m_renderID);
		switch (type)
		{
		case TextureType::Texture1D:
		case TextureType::Texture1DArray:
		case TextureType::Texture2D:
		case TextureType::Texture2DArray:
		case TextureType::Texture3D:
			glTextureView(m_renderID, target, srcTexID, internalformat, startMip, 1, startLayer, numlayer);
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for RenderTargetView!");
			break;
		}
		error = glGetError();
		this->viewDesc = srcdesc;
		this->viewDesc.ArraySize = numlayer;
		this->viewDesc.MipLevels = 1;
		this->viewDesc.Format = format;

		uint32_t temp_width = srcdesc.Width;
		uint32_t temp_height = srcdesc.Height;
		for (int i = 0; i < startMip; ++i)
		{
			if (temp_width == 1 || temp_height == 1)
				break;
			temp_width = std::max<unsigned int>(1, (temp_width / 2));
			temp_height = std::max<unsigned int>(1, (temp_height / 2));
		}
		this->viewDesc.Width = temp_width;
		this->viewDesc.Height = temp_height;
		this->viewDesc.Type = type;
	}
	OpenGLRenderTargetView::~OpenGLRenderTargetView()
	{
		glDeleteTextures(1, &m_renderID);
	}
	void OpenGLRenderTargetView::Bind(uint32_t framebufferid, AttachmentType type)
	{
		if (uint32_t(type) <= uint32_t(AttachmentType::Color_4))
		{
			GLenum attachmentIndex = Utils::GetAttachmentType(type);
			bool multisampled = viewDesc.SampleCount > 1;
			GLenum target = Utils::GetTextureTarget(viewDesc.Type, multisampled);

			switch (viewDesc.Type)
			{
			case TextureType::Texture1D:
				//glFramebufferTexture1D(GL_FRAMEBUFFER, attachmentIndex, target, m_renderID, m_startMip);
				glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				break;
			case TextureType::Texture2D:
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentIndex, target, m_renderID, m_startMip);
				//glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				break;
			case TextureType::Texture1DArray:
			case TextureType::Texture2DArray:
			case TextureType::Texture3D:
				//glFramebufferTexture(GL_FRAMEBUFFER, attachmentIndex, m_renderID, m_startMip);
				glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				break;
			}

			GLenum status = glCheckNamedFramebufferStatus(framebufferid,GL_FRAMEBUFFER);
			QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is bad");
		}
		else
			QCAT_CORE_ERROR("RenderTarget Bind Error! : Wrong Type for Binding");
	}
	OpenGLDepthStencilView::OpenGLDepthStencilView(TextureType type,const  Ref<Texture>& texture, TextureFormat format, uint32_t startMip, uint32_t startLayer, uint32_t numlayer)
		:m_renderID(0),m_startMip(startMip)
	{
		this->type = ViewType::DepthStencilView;

		Texture_Desc srcdesc = texture->GetDesc();
		bool multisampled = srcdesc.SampleCount > 1;
		GLenum target = Utils::GetTextureTarget(type, multisampled);
		GLenum internalformat = Utils::GetTextureInternalFormat(format);

		uint32_t bitSize = Utils::GetBitSizeFromFormat(format);
		uint32_t srcBitSize = Utils::GetBitSizeFromFormat(srcdesc.Format);

		//GLenum srcTextureFormat = Utils::GetTextureDataFromFormat(srcdesc.Format);
		uint32_t srcTexMipLevels = srcdesc.MipLevels;
		uint32_t srcTexLayerNum = srcdesc.ArraySize;
		GLuint srcTexID = (GLuint)texture->GetTexture();
		glGenTextures( 1, &m_renderID);
		switch (type)
		{
		case TextureType::Texture1D:
		case TextureType::Texture1DArray:
		case TextureType::Texture2D:
		case TextureType::Texture2DArray:
			glTextureView(m_renderID, target, srcTexID, internalformat, startMip , 1, startLayer, numlayer);
			break;
		default:
			QCAT_CORE_ERROR("Texture View Contruction Error! : Wrong Texture Type for DepthStencilView!");
			break;
		}
		this->viewDesc = srcdesc;
		this->viewDesc.ArraySize = numlayer;
		this->viewDesc.MipLevels = 1;
		this->viewDesc.Format = format;
		uint32_t temp_width = srcdesc.Width;
		uint32_t temp_height = srcdesc.Height;
		for (int i = 0; i < startMip; ++i)
		{
			if (temp_width == 1 || temp_height == 1)
				break;
			temp_width = std::max<unsigned int>(1, (temp_width / 2));
			temp_height = std::max<unsigned int>(1, (temp_height / 2));
		}
		this->viewDesc.Width = temp_width;
		this->viewDesc.Height = temp_height;
		this->viewDesc.Type = type;
	}
	OpenGLDepthStencilView::~OpenGLDepthStencilView()
	{
		glDeleteTextures(1, &m_renderID);
	}

	void OpenGLDepthStencilView::Bind(uint32_t framebufferid, AttachmentType type)
	{
		if (uint32_t(type) > uint32_t(AttachmentType::Color_4))
		{
			GLenum attachmentIndex = Utils::GetAttachmentType(type);
			bool multisampled = viewDesc.SampleCount > 1;
			GLenum target = Utils::GetTextureTarget(viewDesc.Type, multisampled);

			switch (viewDesc.Type)
			{
			case TextureType::Texture1D:
				//glFramebufferTexture1D(GL_FRAMEBUFFER, attachmentIndex, target, m_renderID, m_startMip);
				glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				break;
			case TextureType::Texture2D:
				//glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentIndex, target, m_renderID, m_startMip);
				glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				break;
			case TextureType::Texture1DArray:
			case TextureType::Texture2DArray:
			case TextureType::Texture3D:
				glNamedFramebufferTexture(framebufferid, attachmentIndex, m_renderID, m_startMip);
				//glFramebufferTexture(GL_FRAMEBUFFER, attachmentIndex, m_renderID,m_startMip);
				break;
			}
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			QCAT_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is bad");
		}
		else
			QCAT_CORE_ERROR("DepthStencil Bind Error! : Wrong Type for Binding");
	}
	
	

}