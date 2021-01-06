#pragma once
#include "Shader.h"

namespace QCat
{
	enum class ShaderDataType
	{
		None =0,Float,Float2,Float3,Float4,Mat3,Mat4,Int,Int2,Int3,Int4,Bool
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
		case ShaderDataType::Bool:     return 1;
		}

		QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		unsigned int size;
		unsigned int offset;
		bool normalized;

		BufferElement(ShaderDataType type,const std::string& name,bool normalized = false, unsigned int offset = 0)
			:name(name),type(type),size(ShaderDataTypeSize(type)),offset(offset),normalized(normalized)
		{}

		unsigned int GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3 ;
			case ShaderDataType::Mat4:    return 4;
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			:m_elements(elements)
		{
			CalculateOffsetAndStride();
		}
	    uint32_t GetStride() const { return m_stride; }
	    const std::vector<BufferElement>& GetElements() const { return m_elements; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }
	public:
		virtual void Bind()const=0;
		virtual void UnBind()const=0;
		static Ref<BufferLayout> Create(const std::initializer_list<BufferElement>& elements, Ref<Shader> vertexShader = nullptr);
	protected:
		void CalculateOffsetAndStride()
		{
			unsigned int offset = 0;
			m_stride = 0;
			for (auto& element : m_elements)
			{
				element.offset = offset;
				offset += element.size;
			}
			m_stride = offset;
			
		}
		std::vector<BufferElement> m_elements;
		unsigned int m_stride;
	};
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void SetData(void* data, unsigned int size) = 0;

		virtual const Ref<BufferLayout>& GetLayout() const = 0;
		virtual void SetLayout(Ref<BufferLayout> layout) = 0;


		static Ref<VertexBuffer> Create(unsigned int size);
		static Ref<VertexBuffer> Create(float* vertices, unsigned int size);

	};
	// Currently QCat Only supoort 32-bit index buffer
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer(){}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual unsigned int GetCount() const = 0;
		static Ref<IndexBuffer> Create(unsigned int* indices, unsigned int size=0);
	};
}