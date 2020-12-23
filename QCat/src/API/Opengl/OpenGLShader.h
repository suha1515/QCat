#pragma once
#include <QCat/Renderer/Shader.h>
#include <string>

namespace QCat
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name ,const std::string& vertexSrc, const std::string& pixelSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual const std::string& GetName() const override { return m_name; }

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);	
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSrouces);
	private:
		uint32_t m_renderID;
		std::string m_name;

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