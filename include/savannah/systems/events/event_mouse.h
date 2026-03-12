#ifndef BSE_MOUSEEVENT_H
#define BSE_MOUSEEVENT_H

#include <savannah/systems/events/event.h>

namespace Savannah {
	enum MouseButtonCodes 
	{
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE
	};
	
	class SAVANNAH_API MouseMovedEvent : public Event 
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y)
		{};
		
		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Мышка подвинулась на: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Мышка подвинулась на: %d, %d", m_MouseX, m_MouseY);
//			return out;
		}
		
		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
		
	private:
		float m_MouseX, m_MouseY;
	};
	
	class SAVANNAH_API MouseScrolledEvent : public Event 
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) 
		{};
		
		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Колёсико крутанулось на: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Колёсико крутанулось на: %d, %d", m_XOffset, m_YOffset);
//			return out;
		}
		
		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
		
	private:
		float m_XOffset, m_YOffset;
	};
	
	class SAVANNAH_API MouseButtonEvent : public Event {
	public:
		inline int GetMouseButtonCode() { return m_MouseButtonCode; }
		
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	
	protected:
		MouseButtonEvent(int MouseButtonCode)
			: m_MouseButtonCode(MouseButtonCode) {}

		int m_MouseButtonCode; // look for enum at the top of the file
	};
	
	class SAVANNAH_API MouseButtonPressed : public MouseButtonEvent 
	{
	public:
		MouseButtonPressed(int MouseButtonCode, int repeatCount)
			: MouseButtonEvent(MouseButtonCode), m_RepeatCount(repeatCount) {}
		
		inline int GetRepeatCount() const { return m_RepeatCount; }
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Нажата кнопка: " << m_MouseButtonCode << " (" << m_RepeatCount << "repeats )";
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Нажата кнопка: %d, (%d repeats)", m_MouseButtonCode, m_RepeatCount);
//			return out;
		}
		
		EVENT_CLASS_TYPE(MouseButtonPressed)
		
	private:
		int m_RepeatCount;
	};
	
	class SAVANNAH_API MouseButtonReleased : public MouseButtonEvent 
	{
	public:
		MouseButtonReleased(int MouseButtonCode)
			: MouseButtonEvent(MouseButtonCode) {}
		
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "Отпущена кнопка: " << m_MouseButtonCode;
			return ss.str();
//			std::string out = "";
//			sprintf((char*)out.c_str(), "Отпущена кнопка: %d", m_MouseButtonCode);
//			return out;
		}
		
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}

#endif
