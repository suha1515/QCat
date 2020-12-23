#pragma once
#include "Graphics.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace QCat
{
	class Shader
	{
	public:
		virtual ~Shader() =default;

		virtual void Bind() const = 0;
		virtual void UnBind()const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string shaderSrc);
		static Ref<Shader> Create(const std::string& name,const std::string& vertexSrc, const std::string& pixelSrc, bool compile = false);
		
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		
		bool Exists(const std::string& name) const;
		Ref<Shader> Load(const std::string& filepath); 
		Ref<Shader> Load(const std::string& name ,const std::string& filepath);

		Ref<Shader> Get(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}