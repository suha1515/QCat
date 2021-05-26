#pragma once
#include "QCat/Core/QCatCore.h"
#include "RenderAPI.h"

namespace QCat
{
	class LayoutElement
	{
		friend class ElementProxy;
	public:
		LayoutElement() = default;
		~LayoutElement() = default;

		LayoutElement(ShaderDataType dataType, const std::string& name);
		LayoutElement& operator[](const std::string& key)
		{
			QCAT_CORE_ASSERT(dataType == ShaderDataType::Struct, "Struct only can access member!");
			for (auto& member : members)
			{
				if (member.name == key)
					return member;
			}
			static LayoutElement empty(ShaderDataType::None, "");
			return empty;
		}

		void Add(ShaderDataType dataType, const std::string& name)
		{
			for (auto& member : members)
			{
				QCAT_CORE_ASSERT(member.name != name, "There already same name in member! ");
			}
			members.emplace_back(LayoutElement(dataType, name));
		}
		void ArraySet(ShaderDataType dataType, size_t arrayCount)
		{
			LayoutElement layout(dataType, "");
			members.push_back(std::move(layout));
			this->arrayCount = arrayCount;
		}
		LayoutElement& GetArrayElement()
		{
			return members[0];
		}
		size_t GetOffsetBegin();
		size_t GetOffsetEnd();
		size_t GetSizeInBytes();
		size_t CalcSize(size_t offset_in);
		size_t AdvanceAlignment(size_t offset, size_t baseAlignment);
		bool CrossBaseAlignment(size_t offset, size_t size, size_t baseAlignment);
		void Finalize();

	private:
		size_t offset;
		size_t size;
		size_t column;
		size_t vectorSize;
		std::string name;

		ShaderDataType dataType;
		//std::vector<uint32_t> arraySize;
		size_t arrayCount;
		std::vector<LayoutElement> members;
	};

	class ElementProxy
	{
		friend class LayoutElement;
	public:
		ElementProxy()
			:offset(0), pElement(nullptr), pBytes(nullptr) {}
		ElementProxy(LayoutElement* pElement, char* pBytes, size_t offset)
			:pElement(pElement), pBytes(pBytes), offset(offset) {}
		~ElementProxy() = default;

		ElementProxy operator[](const std::string& key) const
		{
			return { &(*pElement)[key],pBytes,offset };
		}
		ElementProxy operator[](size_t index) const
		{
			QCAT_CORE_ASSERT(pElement->dataType == ShaderDataType::Array, "Only Array can access element through index!");
			QCAT_CORE_ASSERT(index < pElement->arrayCount, "Array Index is over!");
			size_t elementSize = pElement->size / pElement->arrayCount;
			size_t newoffset = offset + elementSize * index;
			return { &(pElement->members[0]),pBytes,newoffset };
		}
		template<typename T>
		operator T () const
		{
			T value;
			static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			{
				size_t vecSize = pElement->vectorSize;
				size_t coloumn = pElement->column;
				size_t dataSize = (4 * vecSize);
				for (int i = 0; i < coloumn; ++i)
				{
					memcpy(reinterpret_cast<char*>(&value) + dataSize * i, pBytes + offset + pElement->offset + (16 * i), dataSize);
				}
			}
			else
			{
				value =  *reinterpret_cast<T*>(pBytes + offset + pElement->offset);
			}
			return value;
			//return *reinterpret_cast<T*>(pBytes + offset + pElement->offset);
		}
		/*template<typename T>
		T operator *() const
		{
			T value;
			static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			{
				size_t vecSize = pElement->vectorSize;
				size_t coloumn = pElement->column;
				size_t dataSize = (4 * vecSize);
				for (int i = 0; i < coloumn; ++i)
				{
					memcpy(reinterpret_cast<char*>(&value) + dataSize * i, pBytes + offset + pElement->offset + (16 * i), dataSize);
				}
			}
			else
			{
				value = *reinterpret_cast<T*>(pBytes + offset + pElement->offset);
			}
			return value;
		}*/
		template<typename T>
		void operator=(const T& rhs)
		{
			static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			size_t coloumn = pElement->column;
			size_t vecSize = pElement->vectorSize;
			size_t dataSize = (4 * vecSize);
			char* pt = reinterpret_cast<char*>(const_cast<T*>(&rhs));
			if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
			{
				for (int i = 0; i < coloumn; ++i)
				{
					memcpy(pBytes + offset + pElement->offset + (16 * i), pt + dataSize * i, dataSize);
				}
			}
			else if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
			{ 
				dataSize = dataSize * coloumn;
				memcpy(pBytes + offset + pElement->offset, pt, dataSize);
			}
			/*T value = static_cast<T*>(*this);
			return value;*/
		}
	private:
		size_t offset = 0;
		LayoutElement* pElement;
		char* pBytes;
	};

	class ElementBuffer
	{
	public:
		ElementBuffer(LayoutElement layout)
			:layout(layout)
		{
			this->layout.Finalize();
			data.resize(this->layout.GetOffsetEnd());
		}
		ElementProxy operator[](const std::string& key)
		{
			return { &layout[key],data.data(),0 };
		}
		void* GetData() { return data.data(); }
		size_t GetSize() { return layout.GetOffsetEnd();}
	private:
		LayoutElement layout;
		std::vector<char> data;
	};
}
