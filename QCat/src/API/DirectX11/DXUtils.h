#pragma once
#include "QCat/Renderer/enums.h"
#include "QCat/Core/Assert.h"
#include "API/DirectX11/QGfxDeviceDX11.h"
namespace QCat
{
	namespace Utils
	{
	
		static UINT GetMemoryPitch(DXGI_FORMAT format)
		{
			switch (format)
			{
				//8bit
			case DXGI_FORMAT::DXGI_FORMAT_R8_UNORM:					 return 1;
				//16bit
			case DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM:				 return 2;
			case DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT:				 return 2;
				//32 bit
			case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM:			 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:		 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_SNORM:			 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_SINT:					 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT:				 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS:			 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS:				 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT:				 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT:		 return 8;
			case DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT:		 return 16;		

			}
		}
		static DXGI_FORMAT GetDirectDataType(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			 return DXGI_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::RGBA8_SRGB:		 return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case TextureFormat::RED32_INTEGER:   return DXGI_FORMAT_R32_SINT;
			case TextureFormat::RED32_FLOAT:	 return DXGI_FORMAT_R32_FLOAT;

			case TextureFormat::RGB8:			 return DXGI_FORMAT_R8G8B8A8_UNORM;
			case TextureFormat::DEPTH24STENCIL8: return DXGI_FORMAT_R24G8_TYPELESS;
			case TextureFormat::DEPTH32:		 return DXGI_FORMAT_R32_TYPELESS;

			case TextureFormat::RGBA16_Float:    return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case TextureFormat::RGBA32_Float:    return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case TextureFormat::RG16_Float:		 return DXGI_FORMAT_R16G16_FLOAT;
			//24bit
			case TextureFormat::RGB16_Float:    return DXGI_FORMAT_R16G16B16A16_FLOAT;

			}
		}
		static TextureFormat GetTextureFormatFromDX(DXGI_FORMAT format)
		{
			switch (format)
			{
				//32 bit
			case DXGI_FORMAT_R8G8B8A8_UNORM:		  return TextureFormat::RGBA8;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	  return TextureFormat::RGBA8_SRGB;
			case DXGI_FORMAT_R32_SINT:				  return TextureFormat::RED32_INTEGER;
			case DXGI_FORMAT_R32_FLOAT:				  return TextureFormat::RED32_FLOAT;
			case DXGI_FORMAT_R16G16_FLOAT:			  return TextureFormat::RG16_Float;
			case DXGI_FORMAT_R24G8_TYPELESS:		  return TextureFormat::DEPTH24STENCIL8;
			case DXGI_FORMAT_R32_TYPELESS:			  return TextureFormat::DEPTH32;
			
			case DXGI_FORMAT_R16G16B16A16_FLOAT:	  return TextureFormat::RGBA16_Float;
			case DXGI_FORMAT_R32G32B32A32_FLOAT:	  return TextureFormat::RGBA32_Float;

			//8bit
			case DXGI_FORMAT_R8_UNORM:				  return TextureFormat::R8;
			}
		}
		static D3D11_SRV_DIMENSION GetSRVDimensionFromType(TextureType type,bool multisample)
		{
			switch (type)
			{
			case TextureType::Texture1D:			return D3D11_SRV_DIMENSION_TEXTURE1D;
			case TextureType::Texture1DArray:		return D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			case TextureType::Texture2D:		    return multisample ? D3D11_SRV_DIMENSION_TEXTURE2DMS  : D3D11_SRV_DIMENSION_TEXTURE2D;
			case TextureType::Texture2DArray:       return multisample ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY: D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			case TextureType::TextureCube:		    return D3D11_SRV_DIMENSION_TEXTURECUBE;
			case TextureType::TextureCubeArray:		 return D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
			case TextureType::Texture3D:			 return D3D11_SRV_DIMENSION_TEXTURE3D;
			}
		}
		static D3D11_RTV_DIMENSION GetRTVDimensionFromType(TextureType type, bool multisample)
		{
			switch (type)
			{
			case TextureType::Texture1D:			return D3D11_RTV_DIMENSION_TEXTURE1D;
			case TextureType::Texture1DArray:		return D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			case TextureType::Texture2D:		    return multisample ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			case TextureType::Texture2DArray:       return multisample ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			case TextureType::Texture3D:			 return D3D11_RTV_DIMENSION_TEXTURE3D;
			default:
				QCAT_CORE_ASSERT(false, "Wrong RTV textureType!");
			}
		}
		static D3D11_DSV_DIMENSION GetDSVDimenstionFromType(TextureType type, bool multisample)
		{
			switch (type)
			{
			case TextureType::Texture1D:			return D3D11_DSV_DIMENSION_TEXTURE1D;
			case TextureType::Texture1DArray:		return D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			case TextureType::Texture2D:		    return multisample ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			case TextureType::TextureCube:
			case TextureType::Texture2DArray:       return multisample ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			default:
				QCAT_CORE_ASSERT(false, "Wrong DSV textureType!");
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
		static unsigned long long GetDataSize(TextureFormat format)
		{
			switch (format)
			{
				//32 bit
			case TextureFormat::RGBA8:			 return sizeof(unsigned int);
			case TextureFormat::RGBA8_SRGB:		 return sizeof(unsigned int);

			case TextureFormat::RED32_INTEGER:   return sizeof(int);
			case TextureFormat::RED32_FLOAT:	 return sizeof(float);
			case TextureFormat::RG16_Float:		 return sizeof(float);

				//24 bit
			case TextureFormat::RGB8:			 return sizeof(unsigned int);
			case TextureFormat::DEPTH24STENCIL8: return sizeof(unsigned int);
			case TextureFormat::DEPTH32:		 return sizeof(unsigned int);
		
			case TextureFormat::RGBA16_Float:    return sizeof(float);
			case TextureFormat::RGBA32_Float:    return sizeof(float);

			//24bit
			case TextureFormat::RGB16_Float:    return 8;

			}
		}

		static DXGI_FORMAT MapUsageTypeless(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::DEPTH24STENCIL8:
				return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
			case TextureFormat::DEPTH32:
				return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
			default:
				return GetDirectDataType(format);
			}
		}
		static DXGI_FORMAT MapUsageColored(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::DEPTH24STENCIL8:
				return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case TextureFormat::DEPTH32:
				return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			default:
				return GetDirectDataType(format);
			}
		}
		static DXGI_FORMAT MapUsageTyped(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::DEPTH24STENCIL8:
				return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			case TextureFormat::DEPTH32:
				return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
			default:
				return GetDirectDataType(format);
			}
		}

		static DXGI_FORMAT MapTypeSRV(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			case DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_R32_FLOAT;
			default:
				return format;
			}
		}
		static DXGI_FORMAT MapTypeDSV(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_D32_FLOAT;
			default:
				return format;
			}
		}
	
	}
}