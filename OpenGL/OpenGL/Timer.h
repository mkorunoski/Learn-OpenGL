#ifndef TIMER_H
#define TIMER_H

#include <ctime>

class Timer
{
private:
	std::clock_t lastFrame;
	std::clock_t currentFrame;
	double deltaTime;

public:
	Timer() : lastFrame(0), currentFrame(0), deltaTime(0) {}
	void Start()
	{
		lastFrame = std::clock();
	}

	void Tick()
	{
		currentFrame = std::clock();		
		deltaTime = (currentFrame - lastFrame) / (double)CLOCKS_PER_SEC;
		lastFrame = currentFrame;
	}

	double DeltaTime() { return deltaTime; }
};

#endif