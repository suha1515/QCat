#pragma once
#include "Event.h"
#include <sstream>

namespace QCat
{
	class QCAT_API KeyboardEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyboardEvent(int keycode)
			:m_keycode(keycode){}

		int m_keycode;
	};

	class QCAT_API KeyPressedEvent : public KeyboardEvent
	{
	public:
		KeyPressedEvent(int keycode , int repeatCount)
			: KeyboardEvent(keycode),m_repeatCount(repeatCount){}

		inline int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent : " << m_keycode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_repeatCount;
	};

	class QCAT_API KeyReleasedEvent : public KeyboardEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			:KeyboardEvent(keycode){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class QCAT_API KeyTypedEvent : public KeyboardEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyboardEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypeEvent : " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	private:
		int m_repeatCount;
	};
}