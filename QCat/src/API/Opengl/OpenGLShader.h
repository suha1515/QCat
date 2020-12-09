#pragma once
#include <Renderer/Shader.h>
#include <string>

namespace QCat
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& pixelSrc);
		~OpenGLShader();

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual std::vector<char>& GetData() override;
	};
}