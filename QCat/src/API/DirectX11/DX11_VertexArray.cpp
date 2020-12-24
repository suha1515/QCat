#include "qcpch.h"
#include "DX11_VertexArray.h"

namespace QCat
{
	DX11VertexArray::DX11VertexArray()
	{
	}
	DX11VertexArray::~DX11VertexArray()
	{
	}
	void DX11VertexArray::Bind() const
	{
		for (auto& ref : m_vertexBuffers)
			ref->Bind();

		m_indexBuffer->Bind();
	}
	void DX11VertexArray::UnBind() const
	{
	}
	void DX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		m_vertexBuffers.push_back(vertexBuffer);
	}
	void DX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}
}