// ----------------------------------------------------
// j1PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "j1PerfTimer.h"
#include "SDL\include\SDL_timer.h"

unsigned __int64 j1PerfTimer::frequency = 0;

// ---------------------------------------------
j1PerfTimer::j1PerfTimer()
{
	if(frequency == 0)
		frequency = SDL_GetPerformanceFrequency();

	Start();
}

// ---------------------------------------------
void j1PerfTimer::Start()
{
	started_at = SDL_GetPerformanceCounter();
	is_paused = false;
}

// ---------------------------------------------
double j1PerfTimer::ReadMs() const
{
	Uint64 perftime = is_paused ? paused_at : SDL_GetPerformanceCounter();
	return 1000.0 * (double(perftime - started_at) / double(frequency));
}

// ---------------------------------------------
unsigned __int64 j1PerfTimer::ReadTicks() const
{
	return is_paused ? paused_at - started_at : SDL_GetPerformanceCounter() - started_at;
}

double j1PerfTimer::StartedAt()
{
	return started_at;
}

void j1PerfTimer::Pause()
{
	paused_at = SDL_GetPerformanceCounter();
	is_paused = true;
}

void j1PerfTimer::Reset()
{
	paused_at = 0;
	started_at = 0;
	is_paused = true;
}


