#include "qcpch.h"
#include "Shader.h"
#include "Renderer.h"

#include "API/DirectX11/DX11_Shader.h"
#include "API/Opengl/OpenGLShader.h"

namespace QCat
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    QCAT_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
		case RenderAPI::API::DirectX11: //return DXShader::CreateShaderFromFile(filepath);
			break;
		}

		QCAT_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name ,const std::string& vertexSrc, const std::string& pixelSrc, const std::string& geometrySrc)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:
			QCAT_CORE_ASSERT(false, "RendererAPI is none!");
			break;
		case RenderAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name,vertexSrc, pixelSrc);
			break;
		case RenderAPI::API::DirectX11:
			{
			return std::make_shared<DXShader>(name,vertexSrc, pixelSrc, geometrySrc);
			break;

			}	
		}
		QCAT_CORE_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		QCAT_CORE_ASSERT(!Get().Exists(name), "Shader already exists!");
		Get().m_Shaders[name] = shader;
	}
	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto&name = shader->GetName();
		Add(name, shader);
	}
	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name)!=m_Shaders.end();
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		if (Get().Exists(name))
		{
			return Get().GetShader(name);
		}
		else
		{
			auto shader = Shader::Create(filepath);
			Add(name, shader);
			return shader;
		}
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexFile, const std::string& pixelFile, const std::string& gsFile)
	{
		if (Get().Exists(name))
		{
			return Get().GetShader(name);
		}
		else
		{
			auto shader = Shader::Create(name, vertexFile, pixelFile, gsFile);
			Add(name, shader);
			return shader;
		}
	}

	Ref<Shader> ShaderLibrary::GetShader(const std::string& name)
	{
		QCAT_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}
}
