#ifndef SAVANNAH_WINDOW_H
#define SAVANNAH_WINDOW_H

#include <savannah/core.h>
#include <savannah/systems/events/event.h>

namespace Savannah
{
	struct WindowProperties
	{
		std::string title = "Default Window Title";
		unsigned int width = 1200;
		unsigned int height = 768;
		bool vsync = true;
		
		WindowProperties(const std::string& _title = "Default Window Title", 
			unsigned int _width = 320 * 4, 
			unsigned int _height = 200 * 4, 
			bool _vsync = true)
			: title(_title), width(_width), height(_height), vsync(_vsync) 
		{}
	};
	
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		
		virtual ~Window() {}
		
		WindowProperties GetProperties() { return m_Properties; }
		virtual std::string GetTitle() { return m_Properties.title; }
		virtual unsigned int GetWidth() { return m_Properties.width; }
		virtual unsigned int GetHeight() { return m_Properties.height; }
		virtual bool IsVSync() { return m_Properties.vsync; }
		
		virtual float GetTime() { return 0.0f; }
		
		// to be virtual
		virtual void* GetNativeWindow() { return nullptr; } // get GLFWwindow pointer
		virtual void OnUpdate(int code = 0) {}
		virtual void SetVSync(bool vsync) {}
		virtual void SetTitle(const std::string& title) {}
		virtual void SetEventCallback(const EventCallbackFn& callback) {}
		
		static Window* Create(const WindowProperties& properties); // defined in window implementation
	private:
		WindowProperties m_Properties;
	};
	
	class WindowStack
	{
	public:
		WindowStack() = default;
		~WindowStack() 
		{
			for (size_t i = 0; i < m_Windows.size(); i++)
			{
				delete m_Windows[i];
			}
		};
		
		void Push(Window* window) 
		{
			bool exists = false;
			for (size_t i = 0; i < m_Windows.size(); i++)
			{
				if (m_Windows[i] == window)
				{
					exists = true;
					break;
				}
			}
			
			if (!exists)
			{
				m_Windows.push_back(window);
			}
		};
		
		void Pop(Window* window) 
		{
			bool exists = false;
			for (auto it = m_Windows.begin(); it != m_Windows.end(); it++)
			{
				if (*it == window)
				{
					exists = true;
					delete *it;
					m_Windows.erase(it);
					break;
				}
			}
			if (exists)
			{
				SAVANNAH_CORE_TRACE("Window popped from collection.");
			}
		};
		
	private:
		std::vector<Window*> m_Windows = {};
	};
	
}

#endif
