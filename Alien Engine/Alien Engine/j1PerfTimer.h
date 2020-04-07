#ifndef __j1PERFTIMER_H__
#define __j1PERFTIMER_H__

class j1PerfTimer
{
public:

	// Constructor
	j1PerfTimer();

	void Start();
	void Pause();
	void Reset();
	double ReadMs() const;
	unsigned __int64 ReadTicks() const;
	double StartedAt();

private:
	bool is_paused = false; // true fan
	unsigned __int64	started_at;
	unsigned __int64	paused_at;
	static unsigned __int64 frequency;
};

#endif //__j1PERFTIMER_H__