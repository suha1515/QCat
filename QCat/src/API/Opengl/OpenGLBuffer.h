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
}