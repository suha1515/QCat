#pragma once
#include "QCatCore.h"

namespace QCat
{
	class QCAT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be Defined in Client
	Application* CreateApplication();
}


