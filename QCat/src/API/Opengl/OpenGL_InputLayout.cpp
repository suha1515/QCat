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
		uint32_t index = 0;
			for (const auto& element : m_elements)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				GetStride(),
				(const void*)element.offset);
			index++;
		}
	}
	OpenGL_InputLayout::~OpenGL_InputLayout()
	{
	}
	void OpenGL_InputLayout::Bind() const
	{
	}
	void OpenGL_InputLayout::UnBind() const
	{
	}
}