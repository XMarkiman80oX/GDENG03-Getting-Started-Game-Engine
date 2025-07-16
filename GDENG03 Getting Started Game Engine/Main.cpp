#include "AppWindow.h"
#include "InputSystem.h"

int main() {

	try {
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...) 
	{
		return -1;
	} // Handle graphics engine creation failure
	
	{	// Initialize the application window and run the main loop
		AppWindow app;
		if (app.init()) {

			while (app.isRun()) {
				app.broadcast();
			}
		}
	}

	GraphicsEngine::release(); // Clean up graphics engine resources
	InputSystem::release(); // Clean up input system resources
	return 0;
}