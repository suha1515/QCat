#pragma once
#include <Renderer/Graphics.h>
#include <glad/glad.h>

namespace QCat
{
	class QCatOpengl : public Graphics
	{
	public:
		QCatOpengl() = default;
		~QCatOpengl();

		void Initialize();
		void MakeCurrent();
	public:
		virtual void Init(void* pHandle) override;
		virtual void Begin() override;
		virtual void End() override;
	public:
		static HGLRC rc;
	private:
		HDC dc;
	};
}