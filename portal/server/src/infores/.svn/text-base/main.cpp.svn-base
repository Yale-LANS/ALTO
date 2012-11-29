#include <iostream>
#include <vector>
#include <string>

#include "info_resource_base.h"
#include "info_resource_array.h"
#include "info_resource_entity.h"
#include "address_group.h"
#include "alto_directory.h"
#include "network_map.h"
#include "cost_map.h"

int main()
{
	// construction
	// we can build a document piece by piece...
	InfoResBase objAPA;

	objAPA.addString("Name", "Schlafly American Pale Ale");
	objAPA.addString("Name", "Schlafly American Pale Ale");
	objAPA.addString("Origin", "St. Louis, MO, USA");
	objAPA.addNumber("ABV", 3.8);
	objAPA.addBoolean("BottleConditioned", true);

	InfoResBase objBPB;
	objBPB.addString("Name", "John Smith's Extra Smooth");
	objBPB.addString("Origin", "Tadcaster, Yorkshire, UK");
	objBPB.addNumber("ABV", 3.8);
	objBPB.addBoolean("BottleConditioned", false);


	InfoResArray arrayBeer;
	arrayBeer.insert(objAPA);
	arrayBeer.insert(objBPB);

	InfoResBase objDocument;
	objDocument.addArray("Delicious Beers", arrayBeer);
	objDocument.addObject("aaa", objAPA);

	std::cout << objDocument.toJson() << std::endl;

	std::cout << "----------- Address Group -----------" << std::endl;
	// address group
	EndpointAddrGroup group;
	std::vector<const char*> ipv4;
	ipv4.push_back("192.0.2.0/24");
	ipv4.push_back("198.51.100.0/25");
	std::vector<const char*> ipv6;
	ipv6.push_back("2001:db8:0:1::/64");
	ipv6.push_back("2001:db8:0:2::/64");
	group.setIPv4(ipv4);
	group.setIPv6(ipv6);
	std::cout << group.toJson() << std::endl;

	std::cout << "----------- Resource Directory -----------" << std::endl;
	std::vector<ResourceEntry> res(5);
	
	res[0].addUri("http://alto.example.com/networkmap");
	std::vector<const char*> mtypes;
	mtypes.push_back("application/alto-networkmap+json");
	res[0].addMediaTypes(mtypes);
	std::vector<const char*> accepts;

	res[1].addUri("http://alto.example.com/costmap/num/routingcost");
	mtypes.clear();
	mtypes.push_back("application/alto-costmap+json");
	res[1].addMediaTypes(mtypes);
	accepts.clear();

	res[2].addUri("http://alto.example.com/costmap/num/hopcount");
	mtypes.clear();
	mtypes.push_back("application/alto-costmap+json");
	res[2].addMediaTypes(mtypes);
	accepts.clear();

	res[3].addUri("http://custom.alto.example.com/maps");
	mtypes.clear();
	mtypes.push_back("application/alto-networkmapfilter+json");
	mtypes.push_back("application/alto-costmapfilter+json");
	res[3].addMediaTypes(mtypes);
	accepts.clear();
	accepts.push_back("application/alto-networkmapfilter+json");
	accepts.push_back("application/alto-costmapfilter+json");
	res[3].addAccepts(accepts);

	res[4].addUri("http://alto.example.com/endpointprop/lookup");
	mtypes.clear();
	mtypes.push_back("application/alto-endpointprop+json");
	res[4].addMediaTypes(mtypes);
	accepts.clear();
	accepts.push_back("application/alto-endpointpropparams+json");
	res[4].addAccepts(accepts);

	Capability cap;
	std::vector<const char*> vec;

	vec.push_back("numerical");
	cap.addCapability("cost-modes", vec);
	vec.clear(); 
	vec.push_back("routingcost");
	cap.addCapability("cost-types", vec);
	res[1].addCapability(cap);
	vec.clear();
	cap.clear();

	vec.push_back("numerical");
	cap.addCapability("cost-modes", vec);
	vec.clear(); 
	vec.push_back("hopcount");
	cap.addCapability("cost-types", vec);
	res[2].addCapability(cap);
	vec.clear();
	cap.clear();

	vec.push_back("pid");
	cap.addCapability("prop-types", vec);
	res[4].addCapability(cap);

	InfoResourceDirectory ifd;
	ifd.addResources(res);

	std::cout << ifd.toJson() << std::endl;

	std::cout << "----------- Network Map -------------" << std::endl;
	NetworkMap nm;
	nm.addPid("PID1", group);
	
	InfoResourceNetworkMap inm("1266506139");
	inm.addMap(nm);

	InfoResourceEntity ire;
	InfoResourceMetaData empty;
	ire.setMeta(empty);
	ire.setData(inm);

	std::cout << ire.toJson() << std::endl;

	std::cout << "----------- Cost Map -------------" << std::endl;
	InfoResourceCostMap ircm;
	ircm.addCostMode("numerical");
	ircm.addCostType("routingcost");
	ircm.addVertionTag("1266506139");

	CostMapData cmd;
	DstCosts dcost;

	dcost.addDst("PID1", 1);
	dcost.addDst("PID2", 5);
	dcost.addDst("PID3", 10);
	cmd.addDstCost("PID1", dcost);
	
	dcost.clear();
	dcost.addDst("PID1", 5);
	dcost.addDst("PID2", 1);
	dcost.addDst("PID3", 15);
	cmd.addDstCost("PID2", dcost);

	dcost.clear();
	dcost.addDst("PID1", 20);
	dcost.addDst("PID2", 15);
	dcost.addDst("PID3", 1);
	cmd.addDstCost("PID3", dcost);

	ircm.addCostMapData(cmd);
	ire.setData(ircm);

	std::cout << ire.toJson() << std::endl;
}
