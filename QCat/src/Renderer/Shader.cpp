#include "qcpch.h"
#include "Shader.h"
#include "Renderer.h"

#include "API/DirectX11/DX11_Shader.h"
#include "API/Opengl/OpenGLShader.h"

namespace QCat
{
	Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    QCAT_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return new OpenGLShader(filepath);
		}

		QCAT_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& pixelSrc,bool compile )
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:
			QCAT_CORE_ASSERT(false, "RendererAPI is none!");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLShader(vertexSrc, pixelSrc);
			break;
		case RenderAPI::API::DirectX11:
			return new DX11Shader(vertexSrc, pixelSrc,compile);
			break;
		}
		QCAT_CORE_ASSERT(false, "Unknown API!");
		return nullptr;
	}
}
