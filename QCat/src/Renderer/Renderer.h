#pragma once

namespace QCat
{
	enum class RenderAPI
	{
		None =0,OpenGL =1,DirectX11 = 2
	};
	class Renderer
	{
	public:
		inline static RenderAPI GetAPI(){ return s_renderAPI; }
	private:
		static RenderAPI s_renderAPI;
	};
}