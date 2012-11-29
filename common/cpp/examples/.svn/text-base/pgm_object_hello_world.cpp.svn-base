#include <p4p/p4p.h>
#include <p4p/app/p4papp.h>
#include <vector>

using namespace p4p;
using namespace p4p::app;

int main() {

	// Initialize ISP information
	ISP my_isp("p4p-4.cs.yale.edu", 6671);
	my_isp.loadP4PInfo();

	// Lookup PID for a single client
	int pid1 = my_isp.lookup("128.36.34.1");

	// Initialize guidance
	PeeringGuidanceMatrix my_guidance(&my_isp, "p4p-5.cs.yale.edu", 6673);
	my_guidance.compute();

	// Retrieve peering weights from pid1
	double intrapid_pct, intraisp_pct;
	std::vector<double> weights;
	my_guidance.getWeights(pid1, intrapid_pct, intraisp_pct, weights);

	// Determine total number of PIDs, and also how many are intra-ISP PIDs
	unsigned int num_intraisp_pids, num_total_pids;
	my_isp.getNumPIDs(&num_intraisp_pids, &num_total_pids);

	// Print weights to other intra-ISP PIDs
	for (unsigned int i = 0; i < num_intraisp_pids; ++i)
		std::cout << "Weight to intra-ISP PID " << i << " is " << weights[i] << std::endl;

	// Print weights to external PIDs
	for (unsigned int i = num_intraisp_pids; i < num_total_pids; ++i)
		std::cout << "Weight to external PID " << i << " is " << weights[i] << std::endl;

	return 0;
}

