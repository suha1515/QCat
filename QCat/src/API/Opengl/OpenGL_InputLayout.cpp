#include "qcpch.h"
#include "OpenGL_InputLayout.h"
#include <glad/glad.h>

namespace QCat
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case QCat::ShaderDataType::Float:    return GL_FLOAT;
		case QCat::ShaderDataType::Float2:   return GL_FLOAT;
		case QCat::ShaderDataType::Float3:   return GL_FLOAT;
		case QCat::ShaderDataType::Float4:   return GL_FLOAT;
		case QCat::ShaderDataType::Mat3:     return GL_FLOAT;
		case QCat::ShaderDataType::Mat4:     return GL_FLOAT;
		case QCat::ShaderDataType::Int:      return GL_INT;
		case QCat::ShaderDataType::Int2:     return GL_INT;
		case QCat::ShaderDataType::Int3:     return GL_INT;
		case QCat::ShaderDataType::Int4:     return GL_INT;
		case QCat::ShaderDataType::Bool:     return GL_BOOL;
		}

		QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	OpenGL_InputLayout::OpenGL_InputLayout(const std::initializer_list<BufferElement>& elements)
		:BufferLayout(elements)
	{
		//레이아웃인덱스  ,원소갯수,데이터타입,노말라이즈,스트라이드(데이터 너비),오프셋
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	}
	OpenGL_InputLayout::~OpenGL_InputLayout()
	{
	}
	void OpenGL_InputLayout::Bind() const
	{
		uint32_t index = 0;
		for (const auto& element : m_elements)
		{
			//glEnableVertexAttribArray(index);
			//glVertexAttribPointer(index,
			//	element.GetComponentCount(),
			//	ShaderDataTypeToOpenGLBaseType(element.type),
			//	element.normalized ? GL_TRUE : GL_FALSE,
			//	GetStride(),
			//	(const void*)element.offset);
			//index++;
			switch (element.type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.type),
					element.normalized ? GL_TRUE : GL_FALSE,
					GetStride(),
					(const void*)element.offset);
				index++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(index,
						count,
						ShaderDataTypeToOpenGLBaseType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE,
						GetStride(),
						(const void*)(element.offset + sizeof(float) * count * i));
					glVertexAttribDivisor(index, 1);
					index++;
				}
				break;
			}
			default:
				QCAT_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
	}
	void OpenGL_InputLayout::UnBind() const
	{
	}
}