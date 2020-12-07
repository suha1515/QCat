#pragma once
#include <string>

namespace QCat
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& pixlelSrc);
		~Shader();

		void Bind();
		void UnBind();
	private:
		uint32_t m_renderID;
	};
}