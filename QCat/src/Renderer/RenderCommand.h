#pragma once
#include "RenderAPI.h"

namespace QCat
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_renderAPI->Init();
		}
		inline static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		{
			s_renderAPI->SetViewport(x, y, width, height);
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_renderAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_renderAPI->Clear();
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_renderAPI->DrawIndexed(vertexArray);
		}
		inline static void DrawIndexed(const unsigned int indexCount)
		{
			s_renderAPI->DrawIndexed(indexCount);
		}
		
	private:
		static RenderAPI* s_renderAPI;
	};
}