#ifndef __PERFECTTIMER_H__
#define __PERFECTTIMER_H__

#include "p2Log.h"

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned char uchar;

class PerfectTimer
{
public:

	// Constructor
	PerfectTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	started_at;
	static uint64 frequency;
};

#endif //__j1PERFTIMER_H__
