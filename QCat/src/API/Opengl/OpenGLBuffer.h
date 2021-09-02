#pragma once
#include "QCat/Renderer/Buffer.h"
#include "ElementMap.h"
namespace QCat
{
	using namespace std::string_literals;
	enum class DataFormat
	{
		None=0,Char,UByte,SByte,UShort,SShort,Int,int64,UInt,Uint64,Float,Half
	};
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(unsigned int size);
		OpenGLVertexBuffer(float* vertices, unsigned int size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void SetData(void* data, unsigned int size) override;

		virtual const Ref<BufferLayout>& GetLayout() const {return m_pBufferLayout;}
		virtual void SetLayout(Ref<BufferLayout> layout) { m_pBufferLayout = layout; }
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
	class OpenGLShaderBuffer : public ShaderBuffer
	{
	public:
		OpenGLShaderBuffer(uint32_t size, uint32_t count, void* pData = nullptr);
		virtual ~OpenGLShaderBuffer() {};

		virtual void Bind(uint32_t slot,ShaderType type = ShaderType::NONE) const override;
		virtual void ReadData(std::vector<char>& data) override;
	private:
		uint32_t m_renderID;
	};
}