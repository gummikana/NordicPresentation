#include "simple_profiler.h"

#include <fstream>

void SimpleProfiler::Start()
{
	mStartTime = Poro()->GetUpTime();
#if 0
	std::fstream of("stacktrace.txt", std::ios::out | std::ios::app);
	of << mName << std::endl;
#endif
}