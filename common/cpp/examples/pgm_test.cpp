#include <p4p/p4p.h>
#include <p4p/app/p4papp.h>
#include <vector>

using namespace p4p;
using namespace p4p::app;

int main() {

	ISP* isp = new ISP();
	isp->setDataSourceServer("p4p-8.cs.yale.edu", 6871);
	isp->loadP4PInfo();

	ISPManager isp_mgr;
	isp_mgr.addISP("CT", isp);

	PGMSelectionManager pgm_mgr;
	pgm_mgr.setISPManager(&isp_mgr);
	pgm_mgr.setDefaultOptions(PeeringGuidanceMatrixOptions::FilesharingSwarmDependent());
	pgm_mgr.initGuidance("p4p-8.cs.yale.edu", 6673);

	const PeeringGuidanceMatrix* matrix = pgm_mgr.getGuidanceMatrix(isp);
	std::cout << "Options: " << matrix->getOptions().getOptType() << std::endl; 
	pgm_mgr.releaseGuidanceMatrix(matrix);

	return 0;
}

