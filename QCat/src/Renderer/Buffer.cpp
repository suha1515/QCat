#include "qcpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "API/Opengl/OpenGLBuffer.h"
#include "API/DirectX11/DX11Buffer.h"

namespace QCat
{

	VertexBuffer* VertexBuffer::Create(float* vertices, unsigned int size, void* temp)
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
				if(temp==nullptr)
					QCAT_CORE_ASSERT(size < 0, "DirectX11 VertexBuffer Creation need stride Parameter : temp is nullptr");

				return new DX11VertexBuffer(vertices,size, *(unsigned int*)temp);
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
				return new DX11IndexBuffer(indices,size);
				break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}