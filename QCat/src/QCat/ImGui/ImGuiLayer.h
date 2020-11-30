#pragma once
#include "QCat/Layer/Layer.h"
#include "QCat/Uitiliy/Timer.h"

namespace QCat
{

	class QCAT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		Timer timer;
	};
}