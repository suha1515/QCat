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
		Shader() = default;
		virtual ~Shader() {};

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		void UploadeUniformMat4(const std::string& name,const glm::mat4& matrix)
		{
			GLint location = glGetUniformLocation(m_renderID,name.c_str());
			glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(matrix));
		};
	protected:
		uint32_t m_renderID;
	};
	class VertexShader
	{
	public:
		VertexShader() = default;
		virtual ~VertexShader() {};

		virtual void Bind()=0;
		virtual void UnBind()=0;
		virtual std::vector<char>& GetData() = 0;

		static VertexShader* Create(Graphics& gfx ,const std::string& vertexSrc);
	protected:
		uint32_t m_renderID;
		std::vector<char> data;
	};
	class PixelShader
	{
	public:
		PixelShader() = default;
		virtual ~PixelShader() {};

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual std::vector<char>& GetData() = 0;

		static PixelShader* Create(Graphics& gfx, const std::string& pixlelSrc);
	protected:
		uint32_t m_renderID;
		std::vector<char> data;
	};
}