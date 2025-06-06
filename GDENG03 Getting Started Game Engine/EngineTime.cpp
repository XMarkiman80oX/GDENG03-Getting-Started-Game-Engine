#include "EngineTime.h"

EngineTime* EngineTime::sharedInstance = nullptr;

EngineTime::EngineTime() = default;

EngineTime::~EngineTime() = default;

void EngineTime::initialize() {
	if (!sharedInstance) {
		sharedInstance = new EngineTime();
		sharedInstance->start = std::chrono::system_clock::now();
	}
}

double EngineTime::getDeltaTime() {
	if (!sharedInstance) initialize();
	return sharedInstance->deltaTime;
}

double EngineTime::getTotalElapsedTime() {
	if (!sharedInstance) initialize();
	return sharedInstance->totalElapsedTime;
}

void EngineTime::LogFrameStart() {
	if (sharedInstance) {
		sharedInstance->start = std::chrono::system_clock::now();
	}
}

void EngineTime::LogFrameEnd() {
	if (sharedInstance) {
		sharedInstance->end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = sharedInstance->end - sharedInstance->start;
		sharedInstance->deltaTime = elapsed.count();
		sharedInstance->totalElapsedTime += sharedInstance->deltaTime; // Update total elapsed time
	}
}