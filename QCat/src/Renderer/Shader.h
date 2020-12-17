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

		static Shader* Create(const std::string& vertexSrc, const std::string& pixelSrc);
	};
}