#include "qcpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "API/Opengl/OpenGLBuffer.h"

namespace QCat
{

	VertexBuffer* VertexBuffer::Create(float* vertices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::None:		
			{
				QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
				break;
			}
			case RenderAPI::OpenGL:	
			{
				return new OpenGLVertexBuffer(vertices, size);
				break;
			}
			case RenderAPI::DirectX11:
				break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	IndexBuffer* IndexBuffer::Create(unsigned int* indices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::None:		
			{
				QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
				break;
			}
			case RenderAPI::OpenGL:		
			{
				return new OpenGLIndexBuffer(indices, size);
				break;
			}
			case RenderAPI::DirectX11:
				break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}