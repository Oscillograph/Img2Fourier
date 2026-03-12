#ifndef BSE_KEYEVENT_H
#define BSE_KEYEVENT_H

#include <savannah/systems/events/event.h>

namespace Savannah 
{
	class SAVANNAH_API KeyEvent : public Event 
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		
		EVENT_CLASS_CATEGORY(EventCategoryKeyBoard | EventCategoryInput)
	protected:
		KeyEvent(int KeyCode)
			: m_KeyCode(KeyCode) {}
		
		int m_KeyCode;
	};
	
	class SAVANNAH_API KeyPressedEvent : public KeyEvent 
	{
	public:
		KeyPressedEvent(int KeyCode, int repeatCount)
			: KeyEvent(KeyCode), m_RepeatCount(repeatCount) {}
		
		inline int GetRepeatCount() const { return m_RepeatCount; }
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Нажата клавиша: " << m_KeyCode << " (" << m_RepeatCount << "repeats )";
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Нажата клавиша: %d (%d) repeats", m_KeyCode, m_RepeatCount);
//			return out;
		}
		
		EVENT_CLASS_TYPE(KeyPressed)
		
	private:
		int m_RepeatCount;
	};
	
	class SAVANNAH_API KeyReleasedEvent : public KeyEvent 
	{
	public:
		KeyReleasedEvent(int KeyCode)
			: KeyEvent(KeyCode) {}
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Отпущена клавиша: " << m_KeyCode;
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Отпущена клавиша: %d", m_KeyCode);
//			return out;
		}
		
		EVENT_CLASS_TYPE(KeyReleased)
	};
	
	class SAVANNAH_API KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(int KeyCode)
		: KeyEvent(KeyCode) {}
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Печатается символ: " << m_KeyCode;
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Печатается символ: %d", m_KeyCode);
//			return out;
		}
		
		EVENT_CLASS_TYPE(KeyTyped)
	};
}

#endif
