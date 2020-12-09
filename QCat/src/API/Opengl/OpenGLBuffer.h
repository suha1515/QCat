#pragma once
#include "Renderer/Buffer.h"

namespace QCat
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, unsigned int size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }
 	private:
		unsigned int m_renderID;
		BufferLayout m_layout;
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
}