#pragma once
#include "QCat/Renderer/Buffer.h"
#include "ElementMap.h"
namespace QCat
{
	using namespace std::string_literals;
	enum class DataFormat
	{
		None=0,Char,UByte,SByte,UShort,SShort,Int,int64,UInt,Uint64,Float,Half
	};
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(unsigned int size);
		OpenGLVertexBuffer(float* vertices, unsigned int size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void SetData(void* data, unsigned int size) override;

		virtual const Ref<BufferLayout>& GetLayout() const {return m_pBufferLayout;}
		virtual void SetLayout(Ref<BufferLayout> layout) { m_pBufferLayout = layout; }
 	private:
		unsigned int m_renderID;
		Ref<BufferLayout> m_pBufferLayout;
	};
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual unsigned int GetCount() const { return m_count; }
	private:
		unsigned int m_renderID;
		unsigned int m_count;
	};
	//struct Element
	//{
	//public:
	//	// constructor
	//	Element()
	//		:dataType(ShaderDataType::None) {}
	//	Element(const std::initializer_list<Element>& elements, const std::string& name, size_t arraycount = 0);
	//	Element(const Ref<Element>& element, const std::string& name, size_t arraycount = 0)
	//	{

	//	}
	//	Element(ShaderDataType type, const std::string& name, size_t arraycount = 0)
	//		:dataType(type), arraycount(arraycount), name(name)
	//	{
	//		if (arraycount > 0)
	//		{
	//			dataType = ShaderDataType::Array;
	//			Ref<Element> element = CreateRef<Element>(type, name);
	//			members.push_back(std::move(element));
	//		}
	//	}
	//	// destructor
	//	~Element() = default;
	//	// operator
	//	Element& operator[](const std::string& key)
	//	{
	//		QCAT_CORE_ASSERT(ShaderDataType::Struct == dataType, "Only Struct can Access member element");
	//		for (auto& mem : members)
	//		{
	//			if (mem->name == key)
	//				return *mem;
	//		}
	//		static Element empty{};
	//		return empty;
	//	}
	//	const Element& operator[](const std::string& key) const
	//	{
	//		QCAT_CORE_ASSERT(dataType != ShaderDataType::None, "Dont Access through Empty Element!");
	//		return const_cast<Element&>(*this)[key];
	//	}
	//	void Add(ShaderDataType type, std::string name, size_t count = 0);
	//	//For Array
	//	Element& GetStruct()
	//	{
	//		QCAT_CORE_ASSERT(ShaderDataType::Array == dataType, "Only Struct can Access member element!");
	//		ShaderDataType type = members[0]->dataType;
	//		QCAT_CORE_ASSERT(ShaderDataType::Struct == type, "Array element type isnt struct!");
	//		return *members[0];
	//	}
	//	size_t GetSizeInBytes();
	//	size_t GetOffsetEnd();
	//	size_t GetOffsetBegin();
	//	size_t CalcSize(size_t offset_in);
	//	size_t AdvanceAlignment(size_t offset,size_t basealignment);
	//	size_t offset=0;
	//	size_t size=0;						//if struct size will be all member size (with padding) in struct, if array it will be element size
	//	ShaderDataType dataType;
	//	std::vector<Ref<Element>> members;	//if Type is struct member vector has element if Type is array members[0] is array's element type
	//	size_t arraycount;
	//	std::string name;
	//};
	//class ElementProxy
	//{
	//public:
	//	ElementProxy()
	//		:offset(0),pElement(nullptr),pBytes(nullptr){}
	//	ElementProxy(const Element* pElement, char* pBytes, size_t offset)
	//		:pElement(pElement), pBytes(pBytes), offset(offset){}
	//	~ElementProxy() = default;

	//	ElementProxy operator[](const std::string& key) 
	//	{
	//		return {&(*pElement)[key],pBytes,offset};
	//	}
	//	ElementProxy operator[](const uint32_t index) const
	//	{
	//		QCAT_CORE_ASSERT(pElement->arraycount > 0, "Array only can use index!");
	//		QCAT_CORE_ASSERT(index < pElement->arraycount, "Array Index is over!");
	//		size_t newoffset = pElement->offset + pElement->size * index;
	//		return {  &(*pElement->members[0]),pBytes,newoffset };
	//	}
	//	template<typename T>
	//	operator T& () const
	//	{
	//		static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
	//		return *reinterpret_cast<T*>(pBytes + offset + pElement->offset);
	//	}
	//	operator glm::mat3& () const
	//	{
	//		static_assert(ReverseMapGL<std::remove_const_t<glm::mat3>>::valid, "Unsupported SysType used in conversion");

	//	}
	//	template<typename T>
	//	T& operator=(const T& rhs) const
	//	{
	//		static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
	//		return static_cast<T&>(*this) = rhs;
	//	}
	//private:
	//	size_t offset = 0;
	//	const Element* pElement;
	//	char* pBytes;
	//};
	//class Layout
	//{
	//public:
	//	Layout(const std::string& layoutname);
	//	Layout(std::initializer_list<Element> elements, const std::string& layoutname);
	//	~Layout();

	//	Element& operator[](const std::string& key)
	//	{
	//		return pElement[key];
	//	}
	//	void Add(ShaderDataType type,std::string name,size_t count =0);
	//	void Finalize();
	//
	//private:
	//	Element pElement;
	//};
	//class ElementBuffer
	//{
	//public:
	//	ElementBuffer(Element layout)
	//		:layout(layout){}
	//	ElementProxy operator[](const std::string& key)
	//	{
	//		return { &layout[key],data.data(),0 };
	//	}
	//private:
	//	Element layout;
	//	std::vector<char> data;
	//};
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
			:offset(0),pElement(nullptr),pBytes(nullptr){}
		ElementProxy(LayoutElement* pElement,char* pBytes,size_t offset)
			:pElement(pElement),pBytes(pBytes),offset(offset){}
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
		operator T& () const
		{
			static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			T value;
			size_t vecSize = pElement->vectorSize;
			size_t coloumn = pElement->column;
			size_t dataSize = (4 * vecSize );
			for (int i = 0; i < coloumn; ++i)
			{
				memcpy(reinterpret_cast<char*>(&value) + dataSize * i, pBytes + offset  + pElement->offset+ (16 * i), dataSize);
			}
			return value;
		}
		template<typename T>
		T operator=(const T & rhs)
		{
			static_assert(ReverseMapGL<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			size_t coloumn = pElement->column;
			size_t vecSize = pElement->vectorSize;
			size_t dataSize = (4 * vecSize );
			for (int i = 0; i < coloumn; ++i)
			{
				char* pt =  reinterpret_cast<char*>(const_cast<T*>(&rhs)) + dataSize * i;
				memcpy(pBytes + offset+ pElement->offset + (16 * i), pt, dataSize);
			}
			return static_cast<T&>(*this);
		}
		/*template<>
		operator glm::mat4& () const
		{
			static_assert(ReverseMapGL<std::remove_const_t<glm::mat4>>::valid, "Unsupported SysType used in conversion");
			glm::mat4 value;
			size_t vecSize = pElement->vectorSize;
			size_t coloumn = pElement->column;
			size_t dataSize = (4 * vecSize);
			for (int i = 0; i < coloumn; ++i)
			{
				memcpy(&value[i], pBytes + offset + pElement->offset + (16 * i), dataSize);
			}
			return value;
		}
		template<>
		glm::mat4 operator=(const glm::mat4& rhs)
		{
			static_assert(ReverseMapGL<std::remove_const_t<glm::mat4>>::valid, "Unsupported SysType used in conversion");
			size_t coloumn = pElement->column;
			size_t vecSize = pElement->vectorSize;
			size_t dataSize = (4 * vecSize);
			for (int i = 0; i < coloumn; ++i)
			{
				memcpy(pBytes + offset + pElement->offset + (16 * i), &rhs[i] + dataSize * i, dataSize);
			}
			return static_cast<glm::mat4&>(*this);
		}*/
	private:
		size_t offset = 0;
		LayoutElement* pElement;
		char* pBytes;
	};

	class ElementBuffer
	{
	public:
		ElementBuffer(LayoutElement layout)
			:layout(layout), data(layout.GetOffsetEnd())
		{}
		ElementProxy operator[](const std::string& key)
		{
			return { &layout[key],data.data(),0 };
		}
	private:
		LayoutElement layout;
		std::vector<char> data;
	};
}