#include "qcpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "API/Opengl/OpenGLBuffer.h"
#include "API//Opengl/OpenGL_InputLayout.h"
#include "API/DirectX11/DX11_Buffer.h"
#include "API/DirectX11/DX11_InputLayout.h"

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
	BufferLayout* BufferLayout::Create(const std::initializer_list<BufferElement>& elements, void* vertexShaderCode, unsigned int codeSize)
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
			return new OpenGL_InputLayout(elements);
			break;
		}
		case RenderAPI::DirectX11:
			if (vertexShaderCode == nullptr)
			{
				QCAT_CORE_ASSERT(false, "InputLayout Of DX11 need vertexShaderCode! : shadercode nullptr");
			}
			else if(codeSize<=0)
				QCAT_CORE_ASSERT(false, "InputLayout Of DX11 need codeSize!! : code size<=0");
			return new DX11_InputLayout(elements,vertexShaderCode,codeSize);
			break;

		}
		QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}