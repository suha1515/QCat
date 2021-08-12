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
		static void SetTextureFilter(GLuint texture, GLenum filter, Filtering filtering, Filtering mipFiltering =  Filtering::NONE)
		{
			GLenum glfiltering;
			glfiltering = GetFiltering(filtering, mipFiltering);
			glTextureParameteri(texture, filter, glfiltering);
		}
		static void SetTextureWrapMethod(GLuint texture, GLenum address, WrapingMode mode)
		{
			GLenum wrapmode = GetWrapMode(mode);
			glTextureParameteri(texture, address, wrapmode);
		}
		//Texture
		static uint32_t GetBitSizeFromFormat(TextureFormat format)
		{
			switch (format)
			{
				//64bit
			case TextureFormat::RGBA16_Float:   return 64;
				//128bit
			case TextureFormat::RGBA32_Float:	return 128;
				//48
			case TextureFormat::RGB16_Float:	return 48;
				//32 bit
			case TextureFormat::RGBA8:			return 32;
			case TextureFormat::RGBA8_SRGB:		return 32;
			case TextureFormat::RED32_FLOAT:	return 32;
			case TextureFormat::RED32_INTEGER:  return 32;
			case TextureFormat::RG16_Float:	    return 32;
				//24 bit
			case TextureFormat::RGB8:			return 24;
				//16bit
			case TextureFormat::RG8:			return 16;
				//depth
			case TextureFormat::DEPTH32:	     return 32;
			case TextureFormat::DEPTH24STENCIL8: return 32;
			}
		}
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
				//16bit
			case TextureFormat::RG8:			return GL_UNSIGNED_BYTE;

				//24 bit
			case TextureFormat::RGB8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB16_Float:	return GL_HALF_FLOAT;

			}
		}
		static GLenum GetAttachmentType(AttachmentType type)
		{
			switch (type)
			{
			case AttachmentType::Color_0: return GL_COLOR_ATTACHMENT0;
			case AttachmentType::Color_1: return GL_COLOR_ATTACHMENT1;
			case AttachmentType::Color_2: return GL_COLOR_ATTACHMENT2;
			case AttachmentType::Color_3: return GL_COLOR_ATTACHMENT3;
			case AttachmentType::Color_4: return GL_COLOR_ATTACHMENT4;
			case AttachmentType::Depth:	  return GL_DEPTH_ATTACHMENT;
			case AttachmentType::Depth_Stencil: return GL_DEPTH_STENCIL_ATTACHMENT;
			}
		}
		static GLenum GetTextureTarget(TextureType format, bool multisampled)
		{
			switch (format)
			{
			case TextureType::Texture1D:		  return GL_TEXTURE_1D;
			case TextureType::Texture1DArray:	  return GL_TEXTURE_1D_ARRAY;
			case TextureType::Texture2D:		  return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			case TextureType::Texture2DArray:       return multisampled? GL_TEXTURE_2D_MULTISAMPLE_ARRAY: GL_TEXTURE_2D_ARRAY;
			case TextureType::TextureCube:		     return GL_TEXTURE_CUBE_MAP;
			case TextureType::Texture3D:			 return GL_TEXTURE_3D;
			}
		}
		static GLenum GetTextureTargetFromCube(TextureCubeFace face, bool multisampled)
		{
			switch (face)
			{
			case TextureCubeFace::TextureCube_PositiveX:		  return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			case TextureCubeFace::TextureCube_NegativeX:		  return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			case TextureCubeFace::TextureCube_PositiveY:		  return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			case TextureCubeFace::TextureCube_NegativeY:		  return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			case TextureCubeFace::TextureCube_PositiveZ:		  return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			case TextureCubeFace::TextureCube_NegativeZ:		  return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			}
		}

		static uint32_t GetDimensionFromType(TextureType type)
		{
			switch (type)
			{
			case TextureType::Texture1D:			return 1;
			case TextureType::Texture1DArray:		return 1;
			case TextureType::Texture2D:		    return 2;
			case TextureType::Texture2DArray:       return 2;
			case TextureType::TextureCube:		    return 2;
			case TextureType::Texture3D:			 return 3;
			}
		}
		static unsigned int GetTextureComponentCount(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			 return 4;
			case TextureFormat::RGBA8_SRGB:		 return 4;
			case TextureFormat::RED32_FLOAT:	 return 1;
			case TextureFormat::RED32_INTEGER:   return 1;
			case TextureFormat::RGBA32_Float:	 return 4;
			case TextureFormat::RGBA16_Float:    return 4;
				//24 bit
			case TextureFormat::RGB8:			 return 3;
			case TextureFormat::RG16_Float:	     return 2;
			case TextureFormat::RGB16_Float:	 return 3;
				//16bit
			case TextureFormat::RG8:			 return 2;

				//depth
			case TextureFormat::DEPTH32:		 return 1;
			case TextureFormat::DEPTH24STENCIL8: return 1;
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
				//16bit
			case TextureFormat::RG8:			return GL_RG;

				//depth
			case TextureFormat::DEPTH32:		 return GL_DEPTH_COMPONENT;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
			}

		}
		static GLenum GetTextureDataType(TextureDataType type)
		{
			switch (type)
			{
			case TextureDataType::UNSIGNED_BYTE:	return GL_UNSIGNED_BYTE;
			case TextureDataType::BYTE:				return GL_BYTE;
			case TextureDataType::UNSIGNED_SHORT:	return GL_UNSIGNED_SHORT;
			case TextureDataType::SHORT:			return GL_SHORT;
			case TextureDataType::UNSGINED_INT:		return GL_UNSIGNED_INT;
			case TextureDataType::INT:				return GL_INT;
			case TextureDataType::FLOAT:			return GL_FLOAT;
			}
		}
		static uint32_t GetDataSize(TextureDataType type)
		{
			switch (type)
			{
			case TextureDataType::UNSIGNED_BYTE:	return 1;
			case TextureDataType::BYTE:				return 1;
			case TextureDataType::UNSIGNED_SHORT:	return 2;
			case TextureDataType::SHORT:			return 2;
			case TextureDataType::UNSGINED_INT:		return 4;
			case TextureDataType::INT:				return 4;
			case TextureDataType::FLOAT:			return 4;
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

				//16bit
			case TextureFormat::RG8:			return GL_RG8;

				//depth
			case TextureFormat::DEPTH32:	     return GL_DEPTH_COMPONENT32;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}
		}

		static GLenum GetTextureDataFromFormat(TextureFormat format)
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

				//16bit
			case TextureFormat::RG8:			return GL_UNSIGNED_BYTE;


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

				//16bit
			case GL_RG8:	return TextureFormat::RG8;
				//8bit
			case GL_R8:		return TextureFormat::R8;

				//depth
			case  GL_DEPTH_COMPONENT32:   return TextureFormat::DEPTH32;
			case  GL_DEPTH24_STENCIL8:    return TextureFormat::DEPTH24STENCIL8;
			}
		}

		static GLenum GetAttachmentTypeForCopy(TextureFormat foramat,uint32_t index=0)
		{
			switch (foramat)
			{
			case TextureFormat::DEPTH32: return GL_DEPTH_ATTACHMENT;
			case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
			default:
				return GL_COLOR_ATTACHMENT0 + index;
			}
		}
	}


}