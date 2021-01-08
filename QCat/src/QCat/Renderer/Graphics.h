#pragma once

namespace QCat
{
	class Graphics
	{
	public:
		virtual ~Graphics() = default;

		virtual void Init(void* pHandle) = 0;
		
		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}