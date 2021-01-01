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
		virtual void Init()=0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;
		virtual void SetClearColor(const glm::vec4& color) {};
		virtual void SetClearColor(const DirectX::XMFLOAT4& vec) {};
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0)=0;


		inline static API GetAPI() { return s_api; }
	private:
		static API s_api;
		
	};
}