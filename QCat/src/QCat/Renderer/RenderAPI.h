#pragma once
#include "VertexArray.h"

namespace QCat
{
	class RenderAPI
	{
	public:
		enum class API
		{
			None =0,OpenGL = 1,DirectX11 = 2
		};
	public:
		virtual ~RenderAPI() = default;

		virtual void Init()=0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
		virtual void SetDefaultFrameBuffer()=0;
		virtual void SetClearColor(const glm::vec4& color)=0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0)=0;
		virtual void SetWireFrameMode() = 0;
		virtual void SetFillMode() = 0;


		inline static API GetAPI() { return s_api; }
	private:
		static API s_api;
		
	};
}