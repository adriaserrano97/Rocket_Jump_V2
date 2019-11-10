// ----------------------------------------------------
// j1PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "PerfectTimer.h"
#include "SDL\include\SDL_timer.h"

uint64 PerfectTimer::frequency = 0;

// ---------------------------------------------
PerfectTimer::PerfectTimer()
{
	// TODO 2: Fill Constructor, Start(),ReadMs() and ReadTicks() methods
	// they are simple, one line each!
	Start();
}

// ---------------------------------------------
void PerfectTimer::Start()
{
	frequency = SDL_GetPerformanceFrequency();
	started_at = SDL_GetPerformanceCounter();
}

// ---------------------------------------------
double PerfectTimer::ReadMs() const
{
	return ((((SDL_GetPerformanceCounter() - started_at)) / (float)frequency) * 1000);
}

// ---------------------------------------------
uint64 PerfectTimer::ReadTicks() const
{
	return (SDL_GetPerformanceCounter() - started_at);
}


