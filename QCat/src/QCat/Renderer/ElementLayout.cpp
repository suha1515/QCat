#include "qcpch.h"
#include "ElementLayout.h"

namespace QCat
{
	LayoutElement::LayoutElement(ShaderDataType dataType, const std::string& name)
		:dataType(dataType), name(name)
	{
		switch (dataType)
		{
		case ShaderDataType::Float:    vectorSize = 1; column = 1; break;
		case ShaderDataType::Float2:   vectorSize = 2; column = 1; break;
		case ShaderDataType::Float3:   vectorSize = 3; column = 1; break;
		case ShaderDataType::Float4:   vectorSize = 4; column = 1; break;
		case ShaderDataType::Mat3:     vectorSize = 3; column = 3; break;
		case ShaderDataType::Mat4:     vectorSize = 4; column = 4; break;
		case ShaderDataType::Int:      vectorSize = 1; column = 1; break;
		case ShaderDataType::Int2:     vectorSize = 1; column = 1; break;
		case ShaderDataType::Int3:     vectorSize = 1; column = 1; break;
		case ShaderDataType::Int4:     vectorSize = 1; column = 1; break;
		case ShaderDataType::Bool:     vectorSize = 1; column = 1; break;
		default:
			vectorSize = 1, column = 1;
			break;
		}
	}
	size_t LayoutElement::GetOffsetBegin()
	{
		return offset;
	}
	size_t LayoutElement::GetOffsetEnd()
	{
		size_t offsetEnd = 0;
		switch (dataType)
		{
		case ShaderDataType::Float:    offsetEnd = offset + 4; break;
		case ShaderDataType::Float2:   offsetEnd = offset + 8; break;
		case ShaderDataType::Float3:   offsetEnd = offset + 12; break;
		case ShaderDataType::Float4:   offsetEnd = offset + 16; break;
		case ShaderDataType::Mat3:     offsetEnd = offset + 48; break;
		case ShaderDataType::Mat4:     offsetEnd = offset + 64; break;
		case ShaderDataType::Int:      offsetEnd = offset + 4; break;
		case ShaderDataType::Int2:     offsetEnd = offset + 8; break;
		case ShaderDataType::Int3:     offsetEnd = offset + 12; break;
		case ShaderDataType::Int4:     offsetEnd = offset + 16; break;
		case ShaderDataType::Bool:     offsetEnd = offset + 4; break;
		case ShaderDataType::Struct:
		{
			//offsetEnd = AdvanceAlignment(members.back().GetOffsetEnd(), 16);	
			offsetEnd = members.back().GetOffsetEnd();
			break;
		}
		case ShaderDataType::Array:
		{
			offsetEnd = offset + AdvanceAlignment(members[0].GetSizeInBytes(), 16) * arrayCount;
			break;
		}
		default:
			QCAT_CORE_ASSERT(false, "Invalid Element Size!");
			break;
		}
		return offsetEnd;
	}
	size_t LayoutElement::GetSizeInBytes()
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}
	size_t LayoutElement::CalcSize(size_t offset_in)
	{
		size_t baseAlignment=0;
		switch (dataType)
		{
		case ShaderDataType::Float:    baseAlignment = 4; size = 4; break;
		case ShaderDataType::Float2:   baseAlignment = 8; size = 8; break;
		case ShaderDataType::Float3:   baseAlignment = 16; size = 12; break;
		case ShaderDataType::Float4:   baseAlignment = 16; size = 16; break;
		case ShaderDataType::Mat3:     
		{
			baseAlignment =16; 
			size = (RenderAPI::GetAPI() == RenderAPI::API::OpenGL ? 48 : 36);
			break;
		}
		case ShaderDataType::Mat4:     baseAlignment = 16; size = 64; break;
		case ShaderDataType::Int:      baseAlignment = 4; size = 4; break;
		case ShaderDataType::Int2:     baseAlignment = 8; size = 8; break;
		case ShaderDataType::Int3:     baseAlignment = 16; size = 12; break;
		case ShaderDataType::Int4:     baseAlignment = 16; size = 16; break;
		case ShaderDataType::Bool:     baseAlignment = 4; size = 4; break;
		case ShaderDataType::Struct:
		{
			QCAT_CORE_ASSERT(members.size() > 0, "Struct is empty!");
			offset = AdvanceAlignment(offset_in, 16);
			size_t offsetNext = offset;
			for (auto& member : members)
			{
				offsetNext = member.CalcSize(offsetNext);
			}
			size = GetSizeInBytes();
			return offsetNext;
		}
		case ShaderDataType::Array:
		{
			QCAT_CORE_ASSERT(arrayCount > 0, "Array size is Zero");
			offset = AdvanceAlignment(offset_in, 16);
			members[0].CalcSize(offset);
			size = GetSizeInBytes();
			return offset + size;
		}
		}
		if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			offset = AdvanceAlignment(offset_in, baseAlignment);
		}
		else if(RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			offset = CrossBaseAlignment(offset_in, size, 16) ? AdvanceAlignment(offset_in, 16) : offset_in;
		}
		//else datatype
		return offset + size;
	}
	size_t LayoutElement::AdvanceAlignment(size_t offset, size_t baseAlignment)
	{
		return offset + (baseAlignment - (offset % baseAlignment)) % baseAlignment;
	}
	bool LayoutElement::CrossBaseAlignment(size_t offset,size_t size,size_t baseAlignment)
	{
		size_t end = offset + size;
		size_t startAlignment = offset / baseAlignment;
		size_t endAlignment = end / baseAlignment;
		return (startAlignment != endAlignment && end % baseAlignment !=0u) || size>16;
	}
	void LayoutElement::Finalize()
	{
		CalcSize(0);
	}
}