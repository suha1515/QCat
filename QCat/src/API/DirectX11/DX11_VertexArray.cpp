#include "qcpch.h"
#include "DX11_VertexArray.h"

namespace QCat
{
	DX11VertexArray::DX11VertexArray(const std::string& meshname)
	{
		QCAT_PROFILE_FUNCTION();
		this->meshname = meshname;

	}
	DX11VertexArray::~DX11VertexArray()
	{
	}
	void DX11VertexArray::Bind() const
	{
		QCAT_PROFILE_FUNCTION();

		for (auto& ref : m_vertexBuffers)
			ref->Bind();

		m_indexBuffer->Bind();
	}
	void DX11VertexArray::UnBind() const
	{
	}
	void DX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		QCAT_PROFILE_FUNCTION();

		m_vertexBuffers.push_back(vertexBuffer);
	}
	void DX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		QCAT_PROFILE_FUNCTION();

		m_indexBuffer = indexBuffer;
	}
}