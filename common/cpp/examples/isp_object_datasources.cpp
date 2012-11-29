#include <p4p/p4p.h>

using namespace p4p;

int main()
{
	/* ISP object initially configured without any datasource */
	ISP my_isp;

	/* Configure Portal Server as a data source and fetch information */
	my_isp.setDataSourceServer("p4p-4.cs.yale.edu", 6671);
	my_isp.loadP4PInfo();

	/* Lookup information about peers */
	int pid1 = my_isp.lookup("128.36.34.1");
	int pid2 = my_isp.lookup("14.35.64.1");
	int dist = my_isp.getPDistance(pid1, pid2);

	/* Configure local files as a data source and fetch information */
	my_isp.setDataSourceFile("pidmap.txt", "pdistance.txt");
	my_isp.loadP4PInfo();

	/* Lookup information about peers */
	pid1 = my_isp.lookup("128.36.34.1");
	pid2 = my_isp.lookup("14.35.64.1");
	dist = my_isp.getPDistance(pid1, pid2);

	return 0;
}

