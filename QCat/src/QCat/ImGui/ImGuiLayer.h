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

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		bool OnMouseButtonPressEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleaseEvent(MouseButtonReleasedEvent& e);
		bool OnMouseButtonMoveEvent(MouseMoveEvent& e);
		bool OnMouseButtonScrollEvent(MouseScrollEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
	//	bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizedEvent(WindowResizeEvent& e);
	private:
		Timer timer;
	};
}