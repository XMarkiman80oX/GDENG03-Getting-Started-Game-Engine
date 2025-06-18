#pragma once

#include <iostream>
#include <chrono>
#include <ctime>

class Window;

class EngineTime
{
public:
	static void initialize();
	static double getDeltaTime();
	static double getTotalElapsedTime(); // New function

private:
	EngineTime();
	~EngineTime();
	EngineTime(EngineTime const&) {};
	EngineTime operator=(EngineTime const&) {};

	static EngineTime* sharedInstance;

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	double deltaTime = 0.0f;
	double totalElapsedTime = 0.0f; // New member to track total time
	static void LogFrameStart();
	static void LogFrameEnd();

	friend class Window;
};