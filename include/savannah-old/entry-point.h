#include <savannah/core.h>
#include <savannah/main-app.h>

extern Savannah::App* Savannah::CreateApplication();

int main(int, char**)
{
	Savannah::App* application = Savannah::CreateApplication();
	
	application->Init();
	
	float time_previous = (float)glfwGetTime();
	// int idle_frames = 0; // unused for some reason
	
	while ((!glfwWindowShouldClose((GLFWwindow*)(application->GetWindow()->GetNativeWindow()))) && !application->doExit){
		glfwWaitEventsTimeout(application->GetFPS());
		
		float time_current = (float)glfwGetTime();
		
		application->ProcessInput();
		application->Logic();
		
		if ((time_current - time_previous) >= application->GetFPS()){
			time_previous = time_current;
			
			application->Render();
			// App::Sound();
		}
	}
	
	application->ShutDown();
	delete application;
	return 0;
}
