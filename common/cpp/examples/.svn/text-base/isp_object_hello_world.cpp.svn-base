#include <p4p/p4p.h>

using namespace p4p;

int main()
{
	ISP my_isp("p4p-4.cs.yale.edu", 6671);
	my_isp.loadP4PInfo();

	int pid1 = my_isp.lookup("128.36.34.1");
	int pid2 = my_isp.lookup("14.35.64.1");
	int dist = my_isp.getPDistance(pid1, pid2);

	return 0;
}

