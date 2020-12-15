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
	};

	class OpenGLVertexShader
	{
	public:
		OpenGLVertexShader(const std::string& vertexSrc);
		~OpenGLVertexShader();

		unsigned int GetVertexShader() { return vertexShader; }
	private:
		unsigned int vertexShader = 0;
	};

	class OpenGLPixelShader
	{
	public:
		OpenGLPixelShader(const std::string& pixelSrc);
		~OpenGLPixelShader();
	public:
		unsigned int GetPixelShader() { return fragmentShader; }
	private:
		unsigned int fragmentShader = 0;
	};
}