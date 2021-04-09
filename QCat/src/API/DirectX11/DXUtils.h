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
				//32 bit
			case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM:			 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:		 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_SINT:					 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT:				 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS:			 return 4;
			case DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS:				 return 4;

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

			//24bit
			case TextureFormat::RGB16_Float:    return DXGI_FORMAT_R16G16B16A16_FLOAT;

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