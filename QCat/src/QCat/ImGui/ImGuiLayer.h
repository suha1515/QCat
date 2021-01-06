#pragma once
#include "QCat/Layer/Layer.h"
#include "QCat/Uitiliy/Timer.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Events/KeyboardEvent.h"
#include "QCat/Events/MouseEvent.h"

namespace QCat
{

	class QCAT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach()	override;
		virtual void OnEvent(Event& event) override;

		void OnBegin();
		void OnEnd();
	private:
		INT64 g_Time = 0;
		INT64 g_TicksPerSecond = 0;
	};
}