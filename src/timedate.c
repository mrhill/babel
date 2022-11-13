#include "timedate.h"

#if bbOS == bbOS_PALMOS

bbU32 bbGetTickCount(void)
{
    const bbU32 ticks = TimGetTicks();
    const bbU16 tps = SysTicksPerSecond();
    if (tps == 100) // special case, for PalmOS 4 / m505 and probably other
    	return ticks * 10;
    else if (tps <= 1000)
        return (ticks<<10)/tps;
    else
        return ticks / (tps>>10);
}

bbU32 bbGetTime(void)
{
    return TimGetSeconds() - 2082715200UL;
}

#endif

/*
#include <sys/time.h>
bbU32 bbGetTickCount(void) {
  struct timeval now;
  gettimeofday(&now, (struct timezone *)NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000; // in ms
}   
*/

