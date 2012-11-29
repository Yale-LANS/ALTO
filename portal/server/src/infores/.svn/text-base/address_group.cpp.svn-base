#include "address_group.h"

void EndpointAddrGroup::setIPv4(std::vector<std::string>& addrs)
{
	if (addrs.size() < 1)
		return;
	InfoResArray array(addrs);
	addArray("ipv4", array);
}

void EndpointAddrGroup::setIPv6(std::vector<std::string>& addrs)
{
	if (addrs.size() < 1)
		return;
	
	InfoResArray array(addrs);
	addArray("ipv6", array);
}

