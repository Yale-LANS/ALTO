#include "network_map.h"

void NetworkMapData::setPID(std::string name, EndpointAddrGroup& eag)
{
	addObject(name.c_str(), eag);
}

void NetworkMapData::setPID(std::string name, std::vector<std::string>& ip_v4, std::vector<std::string>& ip_v6)
{
	EndpointAddrGroup group;
	if (ip_v4.size() > 0)
		group.setIPv4(ip_v4);
	if (ip_v6.size() > 0)
		group.setIPv6(ip_v6);
	if (ip_v4.size() > 0 || ip_v6.size() > 0)
		setPID(name, group);
}


///////////////////////////////////////////////////////////

bool InfoResourceNetworkMap::hasPID(std::string pid)
{
	return (_pids_v4.find(pid) != _pids_v4.end());
}

bool InfoResourceNetworkMap::isIPv6(std::string ip)
{
	return (ip.find(':') != std::string::npos);
}

void InfoResourceNetworkMap::setVerTag(std::string ver_tag)
{
	_ver_tag = ver_tag;
	addString("map-vtag", ver_tag.c_str());
}

void InfoResourceNetworkMap::setMap(NetworkMapData& map)
{
	addObject("map", map);
}

void InfoResourceNetworkMap::addIP(std::string pid, std::string ip)
{
	if (!hasPID(pid))
	{
		std::vector<std::string> ip_v;
		_pids_v4[pid] = ip_v;
		_pids_v6[pid] = ip_v;
	}

	AddrGroup& pids = isIPv6(ip) ? _pids_v6 : _pids_v4;
	pids[pid].push_back(ip);
}

void InfoResourceNetworkMap::commit()
{
	NetworkMapData netmap;

	for(AddrGroup::iterator it = _pids_v4.begin(); it != _pids_v4.end(); it++)
	{
		std::string pid = it->first;
		netmap.setPID(pid.c_str(), _pids_v4[pid], _pids_v6[pid]);
	}

	setMap(netmap);
}

