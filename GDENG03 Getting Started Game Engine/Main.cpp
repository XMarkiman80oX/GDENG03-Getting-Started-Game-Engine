#include "AppWindow.h"

int main() {

	try {
		GraphicsEngine::create();
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
	return 0;
}