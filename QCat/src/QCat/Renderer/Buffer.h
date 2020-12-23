#pragma once

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

		BufferElement(ShaderDataType type,const std::string& name,bool normalized = false)
			:name(name),type(type),size(ShaderDataTypeSize(type)),offset(0),normalized(normalized)
		{}

		unsigned int GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3 * 3;
			case ShaderDataType::Mat4:    return 4 * 4;
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
		inline uint32_t GetStride() const { return m_stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_elements; }
	public:
		virtual void Bind()const=0;
		virtual void UnBind()const=0;

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

		static BufferLayout* Create(const std::initializer_list<BufferElement>& elements, void* vertexShaderCode = nullptr, unsigned int codeSize = 0);
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

		virtual const Ref<BufferLayout>& GetLayout() const = 0;
		virtual void SetLayout(BufferLayout* layout) = 0;

		static VertexBuffer* Create(float* vertices, unsigned int size,void * temp=nullptr);
	};
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer(){}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual unsigned int GetCount() const = 0;
		static IndexBuffer* Create(unsigned int* indices, unsigned int size=0);
	};
}