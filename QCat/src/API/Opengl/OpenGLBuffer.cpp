#include "qcpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace QCat
{
	//VertexBuffer//
	OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, unsigned int size,BufferUsage usage)
	{
		QCAT_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_renderID);
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		
		GLenum bufferUsage;
		switch (usage)
		{
		case BufferUsage::Default: bufferUsage = GL_STATIC_DRAW; break;
		case BufferUsage::Dynamic: bufferUsage = GL_DYNAMIC_DRAW; break;
		case BufferUsage::Immutable: bufferUsage = GL_STATIC_DRAW; break;
		case BufferUsage::Staging : bufferUsage = GL_DYNAMIC_READ;  break;
		}

		glBufferData(GL_ARRAY_BUFFER, size,data, bufferUsage);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_renderID);
	}
	void OpenGLVertexBuffer::Bind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
	}
	void OpenGLVertexBuffer::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void OpenGLVertexBuffer::SetData(void* data, unsigned int size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
	//IndexBuffer//
	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count, BufferUsage usage)
		:m_count(count)
	{
		QCAT_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);

		GLenum bufferUsage;
		switch (usage)
		{
		case BufferUsage::Default: bufferUsage = GL_STATIC_DRAW; break;
		case BufferUsage::Dynamic: bufferUsage = GL_DYNAMIC_DRAW; break;
		case BufferUsage::Immutable: bufferUsage = GL_STATIC_DRAW; break;
		case BufferUsage::Staging: bufferUsage = GL_DYNAMIC_READ;  break;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_renderID);
	}
	void OpenGLIndexBuffer::Bind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
	}
	void OpenGLIndexBuffer::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	OpenGLShaderBuffer::OpenGLShaderBuffer(uint32_t size, uint32_t count, void* pData)
	{
		this->dataSize = size;
		this->count = count;

		glGenBuffers(1, &m_renderID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_renderID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, int(size * count), pData, GL_STATIC_DRAW);
	}

	void OpenGLShaderBuffer::Bind(uint32_t slot, ShaderType type) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_renderID);
	}

	void OpenGLShaderBuffer::ReadData(std::vector<char>& data)
	{
		int size = this->dataSize * this->count;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_renderID);
		char* pData = reinterpret_cast<char*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT));

		for (int i = 0; i < size; ++i)
			data[i] = pData[i];

		//glMapNamedBufferRange(m_renderID,)
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//Layout::Layout(const std::string& layoutname)
	//{
	//	pElement = Element(ShaderDataType::Struct, layoutname);
	//}
	//Layout::Layout(std::initializer_list<Element> elements, const std::string& layoutname)
	//	:pElement(elements, layoutname)
	//{		}
	//Layout::~Layout()
	//{
	//}
	//void Layout::Add(ShaderDataType type, std::string name, size_t count)
	//{
	//	for (auto& member : pElement.members)
	//	{
	//		if (member->name == name)
	//			QCAT_CORE_ASSERT(false, "Same name is in ElementLayout!");
	//	}
	//	pElement.members.emplace_back(CreateRef<Element>(type, name, count));
	//}
	//namespace Utils
	//{
	//	uint32_t GetBaseAlignment(ShaderDataType type)
	//	{
	//		switch (type)
	//		{
	//		case ShaderDataType::Float:    return 4;
	//		case ShaderDataType::Float2:   return 4 * 2;
	//		case ShaderDataType::Float3:   return 4 * 4;
	//		case ShaderDataType::Float4:   return 4 * 4;
	//		case ShaderDataType::Mat3:     return 4 * 4;
	//		case ShaderDataType::Mat4:     return 4 * 4;
	//		case ShaderDataType::Int:      return 4;
	//		case ShaderDataType::Int2:     return 4 * 2;
	//		case ShaderDataType::Int3:     return 4 * 4;
	//		case ShaderDataType::Int4:     return 4 * 4;
	//		case ShaderDataType::Bool:     return 4;
	//		}
	//	}
	//	uint32_t GetDataSize(ShaderDataType type)
	//	{
	//		switch (type)
	//		{
	//		case ShaderDataType::Float:    return 4;
	//		case ShaderDataType::Float2:   return 4 * 2;
	//		case ShaderDataType::Float3:   return 4 * 4;
	//		case ShaderDataType::Float4:   return 4 * 4;
	//		case ShaderDataType::Mat3:     return 4 * 4;
	//		case ShaderDataType::Mat4:     return 4 * 4;
	//		case ShaderDataType::Int:      return 4;
	//		case ShaderDataType::Int2:     return 4 * 2;
	//		case ShaderDataType::Int3:     return 4 * 4;
	//		case ShaderDataType::Int4:     return 4 * 4;
	//		case ShaderDataType::Bool:     return 4;
	//		}
	//	}
	//}
	//void Layout::Finalize()
	//{
	//	pElement.CalcSize(0);
	//}
	///*Ref<Element> AccessRecursivly(Element& element)
	//{
	//	Ref<Element> var = CreateRef<Element>(element.dataType, element.name, element.arraycount);
	//	for (auto& memeber : element.members)
	//	{
	//		var->members = 
	//	}
	//}*/
	//Element::Element(const std::initializer_list<Element>& elements, const std::string& name, size_t arraycount)
	//	:name(name),arraycount(arraycount)
	//{
	//	for (auto& element : elements)
	//	{
	//		Ref<Element> pt = CreateRef<Element>(element.dataType, element.name, element.arraycount);
	//		if(!element.members.empty())
	//			pt->members = element.members;
	//		members.push_back(std::move(pt));
	//	}
	//	dataType = ShaderDataType::Struct;
	//	//QCAT_CORE_ASSERT(false, "Dont Initailize ElementLayout with Initialization list");
	//}
	//void Element::Add(ShaderDataType type, std::string name, size_t count)
	//{
	//	QCAT_CORE_ASSERT(ShaderDataType::Struct == dataType, "Only Struct can Access member element");
	//	QCAT_CORE_ASSERT(dataType != ShaderDataType::None, "Empty Layout Cant add element!");
	//	for (auto& member : members)
	//	{
	//		if (member->name == name)
	//			QCAT_CORE_ASSERT(false, "Same name is in ElementLayout!");
	//	}
	//	members.emplace_back(CreateRef<Element>(type, name, count));
	//}
	//size_t Element::GetSizeInBytes() 
	//{
	//	return GetOffsetEnd() - GetOffsetBegin();
	//}
	//size_t Element::GetOffsetEnd()
	//{
	//	size_t offsetEnd = 0;
	//	switch (dataType)
	//	{
	//	case ShaderDataType::Float:    offsetEnd = offset + 4; break;
	//	case ShaderDataType::Float2:   offsetEnd = offset + 8; break;
	//	case ShaderDataType::Float3:   offsetEnd = offset + 12; break;
	//	case ShaderDataType::Float4:   offsetEnd = offset + 16; break;
	//	case ShaderDataType::Mat3:     offsetEnd = offset + 48; break;
	//	case ShaderDataType::Mat4:     offsetEnd = offset + 64; break;
	//	case ShaderDataType::Int:      offsetEnd = offset + 4; break;
	//	case ShaderDataType::Int2:     offsetEnd = offset + 8; break;
	//	case ShaderDataType::Int3:     offsetEnd = offset + 12; break;
	//	case ShaderDataType::Int4:     offsetEnd = offset + 16; break;
	//	case ShaderDataType::Bool:     offsetEnd = offset + 4; break;
	//	case ShaderDataType::Struct:
	//	{
	//		offsetEnd = AdvanceAlignment(members.back()->GetOffsetEnd(), 16);
	//		glm::mat4 a;
	//		break;
	//	}
	//	case ShaderDataType::Array:
	//	{
	//		offsetEnd = offset + AdvanceAlignment(members[0]->GetSizeInBytes(), 16) * arraycount;
	//		break;
	//	}
	//	default:
	//		QCAT_CORE_ASSERT(false, "Invalid Element Size!");
	//		break;
	//	}
	//	return offsetEnd ;
	//}
	//size_t Element::GetOffsetBegin()
	//{
	//	return offset;
	//}
	//size_t Element::CalcSize(size_t offset_in)
	//{
	//	switch (dataType)
	//	{
	//	case ShaderDataType::Float:    offset = AdvanceAlignment(offset_in, 4);  size = 4; return offset + 4;
	//	case ShaderDataType::Float2:   offset = AdvanceAlignment(offset_in, 8);  size = 8; return offset + 8;
	//	case ShaderDataType::Float3:   offset = AdvanceAlignment(offset_in, 16); size = 12; return offset + 12;
	//	case ShaderDataType::Float4:   offset = AdvanceAlignment(offset_in, 16); size = 16; return offset + 16;
	//	case ShaderDataType::Mat3:     offset = AdvanceAlignment(offset_in, 16); size = 48; return offset + 48;
	//	case ShaderDataType::Mat4:     offset = AdvanceAlignment(offset_in, 16); size = 64; return offset + 64;
	//	case ShaderDataType::Int:      offset = AdvanceAlignment(offset_in, 4);  size = 4; return offset + 4;
	//	case ShaderDataType::Int2:     offset = AdvanceAlignment(offset_in, 8);  size = 8; return offset + 8;
	//	case ShaderDataType::Int3:     offset = AdvanceAlignment(offset_in, 16); size = 12; return offset + 12;
	//	case ShaderDataType::Int4:     offset = AdvanceAlignment(offset_in, 16); size = 16; return offset + 16;
	//	case ShaderDataType::Bool:     offset = AdvanceAlignment(offset_in, 4);  size = 4; return offset + 4;
	//	case ShaderDataType::Struct:
	//	{
	//		QCAT_CORE_ASSERT(members.size() > 0, "Struct is empty!");
	//		offset = AdvanceAlignment(offset_in, 16);
	//		size_t offsetNext = offset;
	//		for (auto& member : members)
	//		{
	//			offsetNext = member->CalcSize(offsetNext);
	//		}
	//		size = GetSizeInBytes();
	//		return offsetNext;
	//	}
	//	case ShaderDataType::Array:
	//	{
	//		QCAT_CORE_ASSERT(arraycount > 0, "Array size is Zero");
	//		offset = AdvanceAlignment(offset_in, 16);
	//		members[0]->CalcSize(offset);
	//		size = GetSizeInBytes();
	//		return offset + size;
	//	}
	//	}
	//}
	//size_t Element::AdvanceAlignment(size_t offset, size_t basealignment)
	//{
	//	return offset + (basealignment - (offset % basealignment)) % basealignment;
	//}
}