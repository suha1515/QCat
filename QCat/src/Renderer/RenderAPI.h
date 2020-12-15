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
		virtual void SetClearColor(const glm::vec4& color) {};
		virtual void SetClearColor(const DirectX::XMFLOAT4& vec) {};
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {};
		virtual void DrawIndexed(const unsigned int indexCount) {};


		inline static API GetAPI() { return s_api; }
	private:
		static API s_api;
		
	};
}