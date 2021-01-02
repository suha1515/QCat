#include "qcpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace QCat
{
	//VertexBuffer without initial data
	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}
	//VertexBuffer//
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, unsigned int size)
	{
		QCAT_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_renderID);
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
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
	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count)
		:m_count(count)
	{
		QCAT_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
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
}