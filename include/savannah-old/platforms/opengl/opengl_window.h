#ifndef SAVANNAH_WINDOWSWINDOW_H
#define SAVANNAH_WINDOWSWINDOW_H

#include <savannah/core.h>
#include <savannah/window.h>

#include <savannah/systems/renderer/graphics_context.h>
#include <savannah/platforms/opengl/opengl_context.h>


namespace Savannah 
{
	class WindowsWindow : public Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		
		WindowsWindow(const WindowProperties& properties);
		~WindowsWindow();
		
		void OnUpdate(int code = 0) override;
		
		inline unsigned int GetWidth() override { return m_Data.width; }
		inline unsigned int GetHeight() override { return m_Data.height; }
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		
		void SetVSync(bool enabled) override;
		bool IsVSync() override;
		
		inline void* GetNativeWindow() override { return m_Window; }
		
		inline float GetTime() override { return (float)glfwGetTime(); }
		
		inline std::string GetTitle() override { return m_Data.title; };
		inline void SetTitle(const std::string& newTitle) override 
		{ 
			m_Data.title = newTitle;
			glfwSetWindowTitle((GLFWwindow*)GetNativeWindow(), newTitle.c_str());
		};
	private:
		void Init(const WindowProperties& properties);
		void Shutdown();
		
		struct WindowData 
		{
			std::string title = "";
			unsigned int width = 640;
			unsigned int height = 360;
			bool VSync = true;
			EventCallbackFn EventCallback = Callback_dummy;
			
			WindowData(const std::string& _title = "Blazing Serpent Engine",
				//unsigned int width = 1280,
				//unsigned int height = 720,
				unsigned int _width = 640,
				unsigned int _height = 360,
				bool _vsync = true,
				EventCallbackFn _eventCallback = Callback_dummy)
			: title(_title), width(_width), height(_height), VSync(_vsync), EventCallback(_eventCallback)
			{}
		};
		
		GLFWwindow* m_Window;
		GraphicsContext* m_GraphicsContext;
		
		WindowData m_Data;
	};
}

#endif

