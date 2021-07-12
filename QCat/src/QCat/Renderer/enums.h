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
		Color,Depth, Depth_Stencil
	};
	enum class AttachmentType
	{
		Color_0,Color_1,Color_2,Color_3,Color_4,
		Depth,Stencil,Depth_Stencil
	};
	enum class TextureType
	{
		None = 0,
		Texture1D,Texture1DArray,
		Texture2D, Texture2DArray, 
		TextureCube,TextureCubeArray,
		Texture3D,
		TextureCube_PositiveX,
		TextureCube_NegativeX,
		TextureCube_PositiveY,
		TextureCube_NegativeY,
		TextureCube_PositiveZ,
		TextureCube_NegativeZ
	};
	enum class TextureFormat
	{
		None = 0,
		// 32 bit
		RGBA8_SRGB,
		RGBA8, RED32_FLOAT,RED32_INTEGER, RG16_Float, RGBA32_Float, RGBA16_Float,
		// 24bit
		RGB8,RGB16_Float,
		// 16bit
		RG8,
		// 8bit
		R8,
		// DepthStencil
		DEPTH24STENCIL8,
		DEPTH32,

	};
	enum class TextureUsage
	{
		Default = 0, Immutable, Dynamic, Staging
	};
	enum class BindFlag
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER,
		SHADER_RESOURCE,
		STREAM_OUTPUT,
		RENDER_TARGET,
		DEPTH_STENCIL,
		UNORDERED_ACCESS,
		VIDEO_ENCODER
	};
	enum class AccessFlag
	{
		CPU_WRITE,CPU_READ
	};
	enum class Resource_Misc_Flag
	{
		MISC_GENERATE_MIPS,
		MISC_SHARED,
		MISC_TEXTURECUBE,
		MISC_DRAWINDIRECT_ARGS,
		MISC_BUFFER_ALLOW_RAW_VIEWS,
		MISC_BUFFER_STRUCTURED,
		MISC_RESOURCE_CLAMP,
		MISC_SHARED_KEYEDMUTEX,
		MISC_GDI_COMPATIBLE,
		MISC_SHARED_NTHANDLE,
		MISC_RESTRICTED_CONTENT,
		MISC_RESTRICT_SHARED_RESOURCE,
		MISC_RESTRICT_SHARED_RESOURCE_DRIVER,
		MISC_GUARDED,
		MISC_TILE_POOL,
		MISC_TILED,
		MISC_HW_PROTECTED
	};
	enum class ResourceType
	{
		None=0,Texture,Data
	};
	enum class DataType
	{
		None, Float, Float2, Float3, Float4, Matrix,
		Int, Int2, Int3, Int4
	};
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Struct, Array
	};
	static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 4;
		}

		QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
}
