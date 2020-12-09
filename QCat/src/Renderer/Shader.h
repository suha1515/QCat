#pragma once
#include "Graphics.h"
namespace QCat
{
	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() {};

		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual std::vector<char>& GetData() = 0;
	protected:
		uint32_t m_renderID;
		std::vector<char> data;
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