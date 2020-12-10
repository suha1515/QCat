#pragma once
#include "Renderer/Buffer.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class QCAT_API DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(float* vertices, unsigned int size,unsigned int stride);
		~DX11VertexBuffer(){}

		virtual void Bind() const override ;
		virtual void UnBind() const override ;

	private:
		std::string m_tag;
		UINT m_stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	};
	class QCAT_API DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(unsigned int* indices ,unsigned int size);
		~DX11IndexBuffer() {};

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual unsigned int GetCount() const override { return m_count; }
	private:
		std::string m_tag;
		UINT m_count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	};
}