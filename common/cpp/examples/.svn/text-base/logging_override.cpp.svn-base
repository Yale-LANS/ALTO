#include <p4p/p4p.h>

#include <time.h>
#include <stdio.h>

using namespace p4p;

void myLogCallback(LogLevel level, const char *msg)
{
	/* Ignore out-of-range log levels */
	if (level < 0 || LOG_ERROR < level)
		return;

	/* Include timestamp */
	time_t curtime = time(NULL);

	/* Output log to standard output */
	printf("time:%lu,type:json,log%d:{%s}\n", (unsigned long)curtime, level, msg);
}

int main()
{
	setLogLevel(LOG_DEBUG);
	setLogOutputCallback(myLogCallback);

	ISP my_isp("p4p-8.cs.yale.edu", 6671);
	my_isp.loadP4PInfo();

	return 0;
}

