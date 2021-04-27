#include "qcpch.h"
#include "Shader.h"
#include "Renderer.h"

#include "API/DirectX11/DX11_Shader.h"
#include "API/Opengl/OpenGLShader.h"

namespace QCat
{
	std::string GetShaderNameFromFile(const std::string& filePath)
	{
		std::string name;
		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');

		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
		name = filePath.substr(lastSlash, count);
		return name;
	}
	Ref<Shader> Shader::Create(const std::string& filepath)
	{

		switch (Renderer::GetAPI())
		{
		case RenderAPI::API::None:    QCAT_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RenderAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
		case RenderAPI::API::DirectX11: return std::make_shared<DXShader>(filepath);
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
			return std::make_shared<OpenGLShader>(name,vertexSrc, geometrySrc,pixelSrc);
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
		QCAT_CORE_ASSERT(!Get_().Exists(name), "Shader already exists!");
		Get_().m_Shaders[name] = shader;
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
		std::string name = GetShaderNameFromFile(filepath);
		if (Get_().Exists(name))
		{
			return Get_().GetShader(name);
		}
		else
		{
			auto shader = Shader::Create(filepath);
			Add(shader);
			return shader;
		}
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		if (Get_().Exists(name))
		{
			return Get_().GetShader(name);
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
		if (Get_().Exists(name))
		{
			return Get_().GetShader(name);
		}
		else
		{
			auto shader = Shader::Create(name, vertexFile, pixelFile, gsFile);
			Add(name, shader);
			return shader;
		}
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		return Get_().GetShader(name);
	}

	Ref<Shader> ShaderLibrary::GetShader(const std::string& name)
	{
		QCAT_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}
}
