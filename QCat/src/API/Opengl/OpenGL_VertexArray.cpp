#include "qcpch.h"
#include "OpenGL_VertexArray.h"

#include <glad/glad.h>

namespace QCat
{
	OpenGLVertexArray::OpenGLVertexArray(const std::string& meshname)
	{
		QCAT_PROFILE_FUNCTION();
		this->meshname = meshname;

		glCreateVertexArrays(1, &m_rendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_rendererID);
	}
	void OpenGLVertexArray::Bind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindVertexArray(m_rendererID);
	}
	void OpenGLVertexArray::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		QCAT_PROFILE_FUNCTION();

		glBindVertexArray(m_rendererID);
		vertexBuffer->Bind();
		vertexBuffer->GetLayout()->Bind();

		m_vertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		QCAT_PROFILE_FUNCTION();

		glBindVertexArray(m_rendererID);
		indexBuffer->Bind();

		m_indexBuffer = indexBuffer;
	}
}