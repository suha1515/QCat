#include "qcpch.h"
#include "Shader.h"
#include "Renderer.h"

#include "API/DirectX11/DX11_Shader.h"
#include "API/Opengl/OpenGLShader.h"

namespace QCat
{

	//VertexShader* VertexShader::Create(Graphics& gfx,const std::string& vertexSrc)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RenderAPI::None:
	//		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
	//		break;
	//	case RenderAPI::OpenGL:
	//		break;
	//	case RenderAPI::DirectX11:
	//		return new DX11VertexShader(gfx, vertexSrc);
	//		break;
	//	}
	//	return nullptr;
	//}
	//PixelShader* PixelShader::Create(Graphics& gfx,const std::string& pixlelSrc)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RenderAPI::None:
	//		QCAT_CORE_ASSERT(false, "RenderAPI is none!"); return nullptr;
	//		break;
	//	case RenderAPI::OpenGL:
	//		break;
	//	case RenderAPI::DirectX11:
	//		return new DX11PixelShader(gfx, pixlelSrc);
	//		break;
	//	}
	//	QCAT_CORE_ASSERT(false, "Unknown RenderAPI!");
	//	return nullptr;
	//}
}
