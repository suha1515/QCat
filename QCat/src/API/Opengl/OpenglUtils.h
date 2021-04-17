#pragma once
#include "QCat/Renderer/enums.h"
#include "QCat/Core/Assert.h"
#include <glad/glad.h>
namespace QCat
{
	namespace Utils
	{
		static GLenum ComparisonToOpengl(COMPARISON_FUNC func)
		{
			switch (func)
			{
			case COMPARISON_FUNC::ALWAYS:			return GL_ALWAYS;
			case COMPARISON_FUNC::GREATER:			return GL_GREATER;
			case COMPARISON_FUNC::GREATER_EQUAL:	return GL_GEQUAL;
			case COMPARISON_FUNC::LESS:				return GL_LESS;
			case COMPARISON_FUNC::LESS_EQUAL:		return GL_LEQUAL;
			case COMPARISON_FUNC::NOT_EQUAL:		return GL_NOTEQUAL;
			case COMPARISON_FUNC::EQUAL:			return GL_EQUAL;
			case COMPARISON_FUNC::NEVER:			return GL_NEVER;
			}
			QCAT_CORE_ASSERT(false, "Wrong ComparisonFunc value.");

		}
		static GLenum GetFilter(Filter filter)
		{
			switch (filter)
			{
			case Filter::MIN: return GL_TEXTURE_MIN_FILTER; break;
			case Filter::MAG: return GL_TEXTURE_MAG_FILTER; break;
				QCAT_CORE_ASSERT(false, "Wrong Filter in Sampler");
			}
		}
		static GLenum GetFiltering(Filtering filtering, Filtering mipFiltering = Filtering::NONE)
		{
			if (mipFiltering != Filtering::NONE)
			{
				switch (mipFiltering)
				{
				case Filtering::POINT:
				{
					switch (filtering)
					{
					case Filtering::POINT:  return GL_NEAREST_MIPMAP_NEAREST; break;
					case Filtering::LINEAR: return GL_NEAREST_MIPMAP_LINEAR; break;
						QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
					}
				}
				case Filtering::LINEAR:
				{
					switch (filtering)
					{
					case Filtering::POINT:  return GL_LINEAR_MIPMAP_NEAREST; break;
					case Filtering::LINEAR: return GL_LINEAR_MIPMAP_LINEAR; break;
						QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");

					}
				}
				QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
				}
			}
			else
			{
				switch (filtering)
				{
				case Filtering::POINT:  return GL_NEAREST; break;
				case Filtering::LINEAR: return GL_LINEAR; break;
					QCAT_CORE_ASSERT(false, "Wrong Filtering in Sampler");
				}
			}
		}
		static GLenum GetWrapMode(WrapingMode mode)
		{
			switch (mode)
			{
			case WrapingMode::REPEAT: return GL_REPEAT;
			case WrapingMode::MIRROR: return GL_MIRRORED_REPEAT;
			case WrapingMode::MIRROR_ONCE: return GL_MIRROR_CLAMP_TO_EDGE;
			case WrapingMode::BORDER: return GL_CLAMP_TO_BORDER;
			case WrapingMode::CLAMP:  return GL_CLAMP_TO_EDGE;
				QCAT_CORE_ASSERT(false, "Wrong Wrapmode in Sampler");
			}
		}
		static void SetSamplerFilter(unsigned int id, GLenum filter, Filtering filtering, Filtering mipFiltering = Filtering::NONE)
		{
			GLenum glfiltering;
			glfiltering = GetFiltering(filtering, mipFiltering);

			glSamplerParameteri(id, filter, glfiltering);
		}
		static void SetSamplerWrap(unsigned int id, GLenum address, WrapingMode mode)
		{
			GLenum wrapmode = GetWrapMode(mode);

			glSamplerParameteri(id, address, wrapmode);
		}
		static void SetTextureFilter(GLenum target, GLenum filter, Filtering filtering, Filtering mipFiltering =  Filtering::NONE)
		{
			GLenum glfiltering;
			glfiltering = GetFiltering(filtering, mipFiltering);
			glTexParameteri(target, filter, glfiltering);
		}
		static void SetTextureWrapMethod(GLenum target, GLenum address, WrapingMode mode)
		{
			GLenum wrapmode = GetWrapMode(mode);
			glTexParameteri(target, address, wrapmode);
		}



		//Texture
		static GLenum GetOpengLDataType(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA8_SRGB:		return GL_UNSIGNED_BYTE;

			case TextureFormat::RED32_FLOAT:	return GL_FLOAT;
			case TextureFormat::RED32_INTEGER:  return GL_INT;
			case TextureFormat::RG16_Float:	    return GL_HALF_FLOAT;
			case TextureFormat::RGBA32_Float:	return GL_FLOAT;
			case TextureFormat::RGBA16_Float:	return GL_HALF_FLOAT;

				//24 bit
			case TextureFormat::RGB8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB16_Float:	return GL_HALF_FLOAT;

			}
		}
		static GLenum GetTextureTarget(TextureType format, bool multisampled)
		{
			switch (format)
			{
			case TextureType::Texture2D:		  return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			case TextureType::TextureArray:       return GL_TEXTURE_2D_ARRAY;
			case TextureType::TextureCube:		  return GL_TEXTURE_CUBE_MAP;
			}
		}
		static GLenum GetTextureFormat(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			 return GL_RGBA;
			case TextureFormat::RGBA8_SRGB:		 return GL_RGBA;
			case TextureFormat::RED32_FLOAT:	 return GL_RED;
			case TextureFormat::RED32_INTEGER:   return GL_RED_INTEGER;
			case TextureFormat::RGBA32_Float:	 return GL_RGBA;
			case TextureFormat::RGBA16_Float:    return GL_RGBA;
				//24 bit
			case TextureFormat::RGB8:			 return GL_RGB;
			case TextureFormat::RG16_Float:	     return GL_RG;
			case TextureFormat::RGB16_Float:	 return GL_RGB;

				//depth
			case TextureFormat::DEPTH32:		 return GL_DEPTH_COMPONENT;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
			}

		}
		static GLenum GetTextureInternalFormat(TextureFormat format)
		{
			switch (format)
			{

				//32 bit
			case TextureFormat::RGBA8:			return GL_RGBA8;
			case TextureFormat::RGBA8_SRGB:		return GL_SRGB8_ALPHA8;
			case TextureFormat::RED32_FLOAT:	return GL_R32F;
			case TextureFormat::RED32_INTEGER:  return GL_R32I;
			case TextureFormat::RG16_Float:	    return GL_RG16F;
			case TextureFormat::RGBA16_Float:   return GL_RGBA16F;
			case TextureFormat::RGBA32_Float:	return GL_RGBA32F;


				//24 bit
			case TextureFormat::RGB8:			return GL_RGB8;
			case TextureFormat::RGB16_Float:	return GL_RGB16F;


				//depth
			case TextureFormat::DEPTH32:	     return GL_DEPTH_COMPONENT32;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}
		}

		static GLenum GetTextureDataFormat(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA8_SRGB:		return GL_UNSIGNED_BYTE;

			case TextureFormat::RED32_FLOAT:	return GL_FLOAT;
			case TextureFormat::RED32_INTEGER:  return GL_INT;
			case TextureFormat::RG16_Float:	    return GL_HALF_FLOAT;
			case TextureFormat::RGBA16_Float:   return GL_HALF_FLOAT;
			case TextureFormat::RGBA32_Float:	return GL_FLOAT;

				//24 bit
			case TextureFormat::RGB8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB16_Float:	return GL_HALF_FLOAT;

				//depth
			case TextureFormat::DEPTH32:		return GL_FLOAT;
			case TextureFormat::DEPTH24STENCIL8:return GL_UNSIGNED_INT_24_8;
			}
		}

		static TextureFormat GetTextureFormatFromGL(GLenum internalformat)
		{
			switch (internalformat)
			{
				//32 bit
			case GL_RGBA8:			 return 	TextureFormat::RGBA8;
			case GL_SRGB8_ALPHA8:	 return 	TextureFormat::RGBA8_SRGB;
			case GL_R32F:			 return 	TextureFormat::RED32_FLOAT;
			case GL_R32I:			 return 	TextureFormat::RED32_INTEGER;
			case GL_RG16F:			 return 	TextureFormat::RG16_Float;
			case GL_RGBA16F:		 return 	TextureFormat::RGBA16_Float;
			case GL_RGBA32F:		 return 	TextureFormat::RGBA32_Float;


				//24 bit
			case GL_RGB8:	 return TextureFormat::RGB8;
			case GL_RGB16F:  return	TextureFormat::RGB16_Float;


				//depth
			case  GL_DEPTH_COMPONENT32:   return TextureFormat::DEPTH32;
			case  GL_DEPTH24_STENCIL8:    return TextureFormat::DEPTH24STENCIL8;
			}
		}
	}


}