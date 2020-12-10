#pragma once
#include <Renderer/Buffer.h>

namespace QCat
{
	class OpenGL_InputLayout : public BufferLayout
	{
	public:
		OpenGL_InputLayout(const std::initializer_list<BufferElement>& elements);
		~OpenGL_InputLayout();

		virtual void Bind() const;
		virtual void UnBind() const;
	};
}