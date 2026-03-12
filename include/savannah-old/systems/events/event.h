#ifndef BSE_EVENT_H
#define BSE_EVENT_H

// get the engine core
#include <savannah/core.h>

namespace Savannah 
{
	// When an event occurs, it gets dispatched and must be dealt with immediately.
	// TODO: stack events and transfer their pack to the game loop to process there.
	
	enum class EventType 
	{
		None = 0,
		
		// window events
		WindowClose, 
		WindowResize, 
		WindowFocus, 
		WindowLostFocus, 
		WindowMoved,
		
		// app events
		AppTick, 
		AppUpdate, 
		AppRender,
		
		// keyboard events
		KeyPressed, 
		KeyReleased,
		KeyTyped,
		
		// mouse events
		MouseButtonPressed, 
		MouseButtonReleased, 
		MouseMoved, 
		MouseScrolled
	};
	
	enum EventCategory 
	{
		None = 0,
		EventCategoryApplication	=	BIT(0),
		EventCategoryInput			=	BIT(1),
		EventCategoryKeyBoard		=	BIT(2),
		EventCategoryMouse			=	BIT(3),
		EventCategoryMouseButton	=	BIT(4)
	};
	
#define EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType(){ return EventType::type; } \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }
	
#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }
	
	class SAVANNAH_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const {return GetName();}
		
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		inline bool IsHandled()
		{
			return m_Handled;
		}
	protected:
		bool m_Handled = false;
	};
	
	
	// --------------------------------------------------------------------
	// 						Event dispatcher usage example:
	// --------------------------------------------------------------------
	// EventDispatcher dispatcher(e);
	// e.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event){
	//	return OnMouseScrolled(event);
	// });
	// --------------------------------------------------------------------
	
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}
		
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
	
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	
	class SAVANNAH_API DummyEvent : public Event 
	{
	public:
		DummyEvent() {}
		
		EVENT_CLASS_TYPE(None)
		EVENT_CLASS_CATEGORY(None)
	};
	
	inline void Callback_dummy(Event& event)
	{ 
		return; 
	}
}

#endif
