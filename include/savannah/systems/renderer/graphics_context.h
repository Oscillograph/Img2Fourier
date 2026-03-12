#ifndef SAVANNAH_GRAPHICSCONTEXT_H
#define SAVANNAH_GRAPHICSCONTEXT_H

#include <savannah/core.h>

namespace Savannah 
{
	// defining a separate context we can attach one renderer per window or its certain area
	class GraphicsContext 
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void CopyBuffers() = 0;
	};	
}

#endif

