#ifndef SAVANNAH_OPENGLCONTEXT_H
#define SAVANNAH_OPENGLCONTEXT_H

#include <savannah/core.h>
#include <savannah/systems/renderer/graphics_context.h>
#include <external/glad/include/glad/glad.h>
#include <external/glfw/glfw3.h>

// struct GLFWwindow;

namespace Savannah
{
	class OpenGLContext : public GraphicsContext 
	{
	public:
		OpenGLContext(GLFWwindow* window);
		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void CopyBuffers() override;
	protected:
		GLFWwindow* m_Window;
	};
}

#endif

