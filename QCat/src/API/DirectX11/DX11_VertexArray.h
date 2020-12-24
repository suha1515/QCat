#pragma once
#include "QCat/Renderer/VertexArray.h"
// this is temporary there are no vertexarray in directx
// but for abstraction class DX11VertexArray include vertexBuffer and IndexBuffer
namespace QCat
{
	class DX11VertexArray : public VertexArray
	{
	public:
		DX11VertexArray();
		~DX11VertexArray();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_vertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }
	private:
		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;
	};

}