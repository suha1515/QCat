#include "qcpch.h"
#include "DX11_BufferLayout.h"

namespace QCat
{
	struct ExtraData
	{
		struct Struct : public ElementLayout::ExtraDataBase
		{
			std::vector<std::pair<std::string, ElementLayout>> elements;
		};
		struct Array : public ElementLayout::ExtraDataBase
		{
			//for mutlple array
			std::optional<ElementLayout> layoutElement;
			uint32_t elementSize;
			uint32_t count;
		};
	};
	ElementLayout::ElementLayout()
	{
	}
	ElementLayout::ElementLayout(ShaderDataType typeIn, uint32_t offset)
		:type(typeIn),offset(offset)
	{
		QCAT_CORE_ASSERT(typeIn != ShaderDataType::None, "ShaderType cant be none!");
		if (typeIn == ShaderDataType::Struct)
		{
			pExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
		}
		else if (typeIn == ShaderDataType::Array)
		{
			pExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
		}
	}
	ElementLayout& ElementLayout::operator[](const std::string& key)
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		QCAT_CORE_ASSERT(ShaderDataType::Struct == type, "Only Struct can access by key");
		for (auto& mem : static_cast<ExtraData::Struct&>(*pExtraData).elements)
		{
			if (mem.first == key)
			{
				return mem.second;
			}
		}
		static ElementLayout empty{};
		return empty;
	}
	const ElementLayout& ElementLayout::operator[](const std::string& key) const
	{
		return const_cast<ElementLayout&>(*this)[key];
	}
	ElementLayout& ElementLayout::Array()
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		QCAT_CORE_ASSERT(ShaderDataType::Array == type, "Only array can access Array()");

		return *static_cast<ExtraData::Array&>(*pExtraData).layoutElement;
	}
	void ElementLayout::Add(ShaderDataType addtype, std::string name, uint32_t offset, uint32_t size)
	{
		QCAT_CORE_ASSERT(ShaderDataType::Struct == type, "Only Struct ShaderDataType can add element");
		auto& structData = static_cast<ExtraData::Struct&>(*pExtraData);
		for (auto& mem : structData.elements)
		{
			if (mem.first == name)
			{
				QCAT_CORE_ASSERT(false,"Same element in the Struct");
			}
		}
		structData.elements.emplace_back(std::move(name), ElementLayout{ addtype });
	}
	void ElementLayout::Set(ShaderDataType addtype, uint32_t count)
	{
		QCAT_CORE_ASSERT(ShaderDataType::Array == type, "Only Array Set ElementType and Count!");
		auto& arrayData = static_cast<ExtraData::Array&>(*pExtraData);
		arrayData.layoutElement = { addtype };
		arrayData.count = count;
	}
	bool ElementLayout::CrossBoundary(uint32_t offset, uint32_t size) noexcept
	{
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
	}
	std::pair<uint32_t, const ElementLayout*> ElementLayout::CalculateIndexingOffset(size_t offset, size_t index) const
	{
		QCAT_CORE_ASSERT(ShaderDataType::Array == type, "Only Array Set ElementType and Count!");
		auto& arrayData = static_cast<ExtraData::Array&>(*pExtraData);
		QCAT_CORE_ASSERT(index < arrayData.count,"Array Index Over!");
		return { offset + arrayData.elementSize * index,&*arrayData.layoutElement };
	}
	size_t ElementLayout::AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept
	{
		return CrossBoundary(offset, size) ? AdvanceBoundary(offset) : offset;
	}
	uint32_t ElementLayout::AdvanceBoundary(uint32_t size) const
	{
		return  size + (16u - size % 16u) % 16u;
	}
	uint32_t ElementLayout::CalculateSize(uint32_t offsetIn)
	{
		switch (type)
		{
		case ShaderDataType::Float:    offset = AdvanceIfCrossesBoundary(offsetIn, 4);  size = 4; return *offset + 4;
		case ShaderDataType::Float2:   offset = AdvanceIfCrossesBoundary(offsetIn, 8);  size = 8; return *offset + 8;
		case ShaderDataType::Float3:   offset = AdvanceIfCrossesBoundary(offsetIn, 12); size = 12; return *offset + 12;
		case ShaderDataType::Float4:   offset = AdvanceIfCrossesBoundary(offsetIn, 16); size = 16; return *offset + 16;
		case ShaderDataType::Mat3:     offset = AdvanceIfCrossesBoundary(offsetIn, 36); size = 36; return *offset + 36;
		case ShaderDataType::Mat4:     offset = AdvanceIfCrossesBoundary(offsetIn, 64); size = 64; return *offset + 64;
		case ShaderDataType::Int:      offset = AdvanceIfCrossesBoundary(offsetIn, 4);  size = 4; return *offset + 4;
		case ShaderDataType::Int2:     offset = AdvanceIfCrossesBoundary(offsetIn, 8);  size = 8; return *offset + 8;
		case ShaderDataType::Int3:     offset = AdvanceIfCrossesBoundary(offsetIn, 12); size = 12; return *offset + 12;
		case ShaderDataType::Int4:     offset = AdvanceIfCrossesBoundary(offsetIn, 16); size = 16; return *offset + 16;
		case ShaderDataType::Bool:     offset = AdvanceIfCrossesBoundary(offsetIn, 4);  size = 4; return *offset + 4;
		case ShaderDataType::Struct:
		{
			auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
			QCAT_CORE_ASSERT((data.elements.size() != 0u),"Struct is empty");
			offset = AdvanceBoundary(offsetIn);
			auto offsetNext = *offset;
			for (auto& el : data.elements)
			{
				offsetNext = el.second.CalculateSize(offsetNext);
			}
			size = offsetNext;
			return offsetNext;
		}
		case ShaderDataType::Array:
		{
			auto& data = static_cast<ExtraData::Array&>(*pExtraData);
			QCAT_CORE_ASSERT(data.count != 0u,"Array size is zero");
			offset = AdvanceBoundary(offsetIn);
			data.layoutElement->CalculateSize(*offset);
			data.elementSize = AdvanceBoundary(data.layoutElement->GetSizeInBytes());
			return GetOffsetEnd(); 
		}
		}
	}
	uint32_t ElementLayout::GetOffsetBegin() const
	{
		return *offset;
	}
	uint32_t ElementLayout::GetOffsetEnd() const
	{
		switch (type)
		{
		case ShaderDataType::Float:    return *offset +  4;
		case ShaderDataType::Float2:   return *offset +  4 * 2;
		case ShaderDataType::Float3:   return *offset +  4 * 3;
		case ShaderDataType::Float4:   return *offset +  4 * 4;
		case ShaderDataType::Mat3:     return *offset +  4 * 3 * 3;
		case ShaderDataType::Mat4:     return *offset +  4 * 4 * 4;
		case ShaderDataType::Int:      return *offset +  4;
		case ShaderDataType::Int2:     return *offset +  4 * 2;
		case ShaderDataType::Int3:     return *offset +  4 * 3;
		case ShaderDataType::Int4:     return *offset +  4 * 4;
		case ShaderDataType::Bool:     return *offset +  4;
		case ShaderDataType::Struct:
		{
			const auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
			return AdvanceBoundary(data.elements.back().second.GetOffsetEnd());
		}
		case ShaderDataType::Array:
		{
			const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
			return *offset + AdvanceBoundary(data.layoutElement->GetSizeInBytes()) * data.count;
		}
		}
	}
	uint32_t ElementLayout::GetSizeInBytes() const
	{
		return GetOffsetEnd() - GetOffsetBegin();
	}
	bool ElementRef::Exists() const noexcept
	{
		if (pLayout->GetDataType() != ShaderDataType::None)
			return true;
		return false;
	}
	ElementRef ElementRef::operator[](const std::string& key) const
	{
		if (pLayout->GetDataType() == ShaderDataType::None)
			return{pLayout,pBytes,offset};
		return{ &(*pLayout)[key],pBytes,offset };
	}
	ElementRef ElementRef::operator[](size_t index) const
	{
		if (pLayout->GetDataType() == ShaderDataType::None)
			return{ pLayout,pBytes,offset };
		const auto indexingData = pLayout->CalculateIndexingOffset(offset, index);
		return { indexingData.second,pBytes,indexingData.first };
	}
	ElementRef::ElementRef(const ElementLayout* pLayout, char* pbytes, uint32_t offset)
		:offset(offset),pLayout(pLayout),pBytes(pbytes)
	{}
	Buffer::Buffer(ElementLayout&& layout)
		: pLayoutRoot(std::move(layout)),
			data(pLayoutRoot.GetOffsetEnd())
	{
	}
	Buffer::Buffer(Buffer&& buf) noexcept
		:pLayoutRoot(std::move(buf.pLayoutRoot)),
		data(std::move(buf.data))
	{
	}
	ElementRef Buffer::operator[](const std::string& key)
	{
		return { &(pLayoutRoot)[key],data.data(),0u };
	}
	char* Buffer::GetData()  noexcept
	{
		return data.data();
	}
	uint32_t Buffer::GetSizeInBytes() const
	{
		return data.size();
	}
	void Buffer::Copy(const Buffer& other)
	{

	}
}
