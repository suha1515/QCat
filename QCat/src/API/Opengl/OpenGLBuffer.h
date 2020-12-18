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

		virtual const Ref<BufferLayout>& GetLayout() const {return m_pBufferLayout;}
		virtual void SetLayout(BufferLayout* layout) { m_pBufferLayout.reset(layout); }
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
}