#pragma once
#include "RenderCommand.h"

namespace QCat
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
		static void Submit(const unsigned int indexCount);
		inline static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }

	};
}