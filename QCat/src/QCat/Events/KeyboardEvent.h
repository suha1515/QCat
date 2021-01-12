#pragma once
#include "Event.h"
#include "QCat/Core/KeyCode.h"
#include <sstream>

namespace QCat
{
	class QCAT_API KeyboardEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyboardEvent(const KeyCode keycode)
			:m_keycode(keycode){}

		KeyCode m_keycode;
	};

	class QCAT_API KeyPressedEvent : public KeyboardEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode , const uint16_t repeatCount)
			: KeyboardEvent(keycode),m_repeatCount(repeatCount){}

		uint16_t GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent : " << m_keycode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint16_t m_repeatCount;
	};

	class QCAT_API KeyReleasedEvent : public KeyboardEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
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
		KeyTypedEvent(const KeyCode keycode)
			: KeyboardEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypeEvent : " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}