#pragma once
namespace QCat
{
	enum class COMPARISON_FUNC
	{
		NEVER, EQUAL,
		LESS, LESS_EQUAL,
		GREATER, GREATER_EQUAL,
		NOT_EQUAL, ALWAYS
	};

	enum class Filtering
	{
		NONE = 0, POINT, LINEAR, ANISOTROPIC
	};
	enum class Filter
	{
		NONE = 0, MIN, MAG, MIP
	};
	enum class FilterMode
	{
		NONE = 0, COMPARISON, MINIMUM, MAXIMUM
	};
	enum class WrapingMode
	{
		REPEAT = 0, MIRROR, CLAMP, BORDER, MIRROR_ONCE
	};
	enum class BufferBit
	{
		Color, Depth, Stencil
	};
	enum class FramebufferUsage
	{
		None = 0,
		Color, Depth, Depth_Stencil
	};
	enum class TextureType
	{
		None = 0,
		Texture2D, TextureArray, TextureCube
	};
	enum class TextureFormat
	{
		None = 0,
		// 32 bit
		RGBA8_SRGB,
		RGBA8, RED32_FLOAT,RED32_INTEGER, RG16_Float, RGBA32_Float, RGBA16_Float,
		// 24bit
		RGB8,
		// DepthStencil
		DEPTH24STENCIL8,
		DEPTH32,

	};

}
