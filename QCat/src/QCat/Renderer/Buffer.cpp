#include "qcpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "API/Opengl/OpenGLBuffer.h"
#include "API//Opengl/OpenGL_InputLayout.h"
#include "API/DirectX11/DX11_Buffer.h"
#include "API/DirectX11/DX11_InputLayout.h"

namespace QCat
{

	VertexBuffer* VertexBuffer::Create(float* vertices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		
			{
				QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
				break;
			}
		case RenderAPI::API::OpenGL:	
			{
				return new OpenGLVertexBuffer(vertices, size);
				break;
			}
		case RenderAPI::API::DirectX11:
			return new DX11VertexBuffer(vertices,size);
			break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	IndexBuffer* IndexBuffer::Create(unsigned int* indices, unsigned int size)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:		
			{
				QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
				break;
			}
		case RenderAPI::API::OpenGL:		
			{
				return new OpenGLIndexBuffer(indices, size);
				break;
			}
		case RenderAPI::API::DirectX11:
				return new DX11IndexBuffer(indices,size);
				break;
		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	BufferLayout* BufferLayout::Create(const std::initializer_list<BufferElement>& elements, Ref<Shader> vertexShader)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:
		{
			QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
			break;
		}
		case RenderAPI::API::OpenGL:
		{
			return new OpenGL_InputLayout(elements);
			break;
		}
		case RenderAPI::API::DirectX11:
			
			return new DX11_InputLayout(elements, std::dynamic_pointer_cast<DXShader>(vertexShader));
			break;

		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}