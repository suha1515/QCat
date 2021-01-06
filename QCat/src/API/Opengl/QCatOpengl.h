#pragma once
#include <QCat/Renderer/Graphics.h>
#include <glad/glad.h>
#include "wgl/wglext.h"

namespace QCat
{
	class QCatOpengl : public Graphics
	{
	public:
		struct WGL
		{
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =NULL;
		};
		QCatOpengl() = default;
		~QCatOpengl()  ;

		void Initialize(HWND hwnd);
		void MakeCurrent();
	public:
		virtual void Init(void* pHandle) override;
		virtual void Begin() override;
		virtual void End() override;
	public:
		static HGLRC rc;
	private:
		HDC dc = NULL;
		WGL wgl;
	};
}