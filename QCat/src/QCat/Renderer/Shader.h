#pragma once
#include "Graphics.h"
#include <glm/glm.hpp>

namespace QCat
{
	enum class ShaderType
	{
		VS = 0, PS, GS, CS
	};
	class Shader
	{
	public:
		virtual ~Shader() =default;

		virtual void Bind() const = 0;
		virtual void UnBind()const = 0;

		// Integer
		virtual void SetInt(const std::string& name, int value ,ShaderType type) = 0;
		virtual void SetIntArray(const std::string& name, int* values, unsigned int count, ShaderType type) = 0;

		// Float
		virtual void SetFloat(const std::string& name, const float& value, ShaderType type) =0;
		virtual void SetFloatArray(const std::string name, float* values, unsigned int count, ShaderType type) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value, ShaderType type) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value, ShaderType type) = 0;

		// Bool
		virtual void SetBool(const std::string& name, const bool& value, ShaderType type) = 0;

		// matrix
		virtual void SetMat4(const std::string& name, const glm::mat4& value, ShaderType type) = 0;
		virtual void SetFloat3u(const std::string& uniformname, const std::string& valuename,const glm::vec3& value) = 0;
		virtual void SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value) = 0;
		virtual void SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& vertexfilepath,const std::string& pixelfilepath);
		static Ref<Shader> Create(const std::string& name,const std::string& vertexSrc, const std::string& pixelSrc);
		

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
