#include "qcpch.h"
#include "OpenGL_UniformBuffer.h"
#include <glad/glad.h>
namespace QCat
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_renderID);
		glNamedBufferData(m_renderID, size, nullptr, GL_DYNAMIC_DRAW);// Need to be aplied about static
		glBindBufferBase(GL_UNIFORM_BUFFER,binding,m_renderID);
	}
	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_renderID);
	}
	void OpenGLUniformBuffer::Bind(uint32_t bindingpoint, Type type )
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, m_renderID);
	}
	void OpenGLUniformBuffer::SetData(const void* pdata, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_renderID, offset, size, pdata);
	}
}